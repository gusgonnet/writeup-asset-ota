/**
 * @file NexUpload.cpp
 *
 * The implementation of download tft file for nextion.
 *
 * @author  Chen Zengpeng (email:<zengpeng.chen@itead.cc>)
 * @date    2016/3/29
 * @copyright
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include "NexUpload.h"

NexUpload::NexUpload(int dummy)
{
    _download_baudrate = 115200;
}

NexUpload::NexUpload(const char *file_name, const uint8_t SD_chip_select, uint32_t download_baudrate)
{
    _file_name = file_name;
    _SD_chip_select = SD_chip_select;
    _download_baudrate = download_baudrate;
}

NexUpload::NexUpload(const String file_Name, const uint8_t SD_chip_select, uint32_t download_baudrate)
{
    NexUpload(file_Name.c_str(), SD_chip_select, download_baudrate);
}

void NexUpload::upload(void)
{
    if (!_checkFile())
    {
        Log.info("the file is error");
        return;
    }
    if (_getBaudrate() == 0)
    {
        Log.info("get baudrate error");
        return;
    }
    if (!_setDownloadBaudrate(_download_baudrate))
    {
        Log.info("modify baudrate error");
        return;
    }
    if (!_downloadTftFile())
    {
        Log.info("download file error");
        return;
    }
    Log.info("download ok");
}

uint16_t NexUpload::_getBaudrate(void)
{
    uint32_t baudrate_array[7] = {115200, 19200, 9600, 57600, 38400, 4800, 2400};
    for (uint8_t i = 0; i < 7; i++)
    {
        if (_searchBaudrate(baudrate_array[i]))
        {
            _baudrate = baudrate_array[i];
            Log.info("get baudrate");
            break;
        }
    }
    return _baudrate;
}

bool NexUpload::_checkFile(void)
{
    spark::Vector<ApplicationAsset> assets = System.assetsAvailable();

    for (auto &localAsset : assets)
    {
        if (localAsset.name() == "nextion.tft")
        {
            Log.info("_checkFile ok");
            asset = localAsset;
            _undownloadByte = localAsset.size();
            return true;
        }
    }

    Log.error("_checkFile failed");
    return false;
}

bool NexUpload::_searchBaudrate(uint32_t baudrate)
{
    String string = String("");
    Serial1.begin(baudrate);
    this->sendCommand("");
    this->sendCommand("connect");
    this->recvRetString(string);
    if (string.indexOf("comok") != -1)
    {
        return 1;
    }
    return 0;
}

void NexUpload::sendCommand(const char *cmd)
{

    while (Serial1.available())
    {
        Serial1.read();
    }

    Serial1.print(cmd);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
}

uint16_t NexUpload::recvRetString(String &string, uint32_t timeout, bool recv_flag)
{
    uint16_t ret = 0;
    uint8_t c = 0;
    long start;
    bool exit_flag = false;
    start = millis();
    while (millis() - start <= timeout)
    {
        while (Serial1.available())
        {
            c = Serial1.read();
            if (c == 0)
            {
                continue;
            }
            string += (char)c;
            if (recv_flag)
            {
                if (string.indexOf(0x05) != -1)
                {
                    exit_flag = true;
                }
            }
        }
        if (exit_flag)
        {
            break;
        }
    }
    ret = string.length();
    return ret;
}

bool NexUpload::_setDownloadBaudrate(uint32_t baudrate)
{
    String string = String("");
    String cmd = String("");

    String filesize_str = String(_undownloadByte);
    String baudrate_str = String(baudrate);
    
    cmd = "whmi-wri " + filesize_str + "," + baudrate_str + ",0";

    Log.info(cmd);

    this->sendCommand("");
    this->sendCommand(cmd.c_str());
    delay(50);
    Serial1.begin(baudrate);
    this->recvRetString(string, 500);
    if (string.indexOf(0x05) != -1)
    {
        return 1;
    }
    return 0;
}

bool NexUpload::_downloadTftFile(void)
{
    char thechar[1];
    int readchar = 0;
    // uint8_t c;
    uint16_t send_timer = 0;
    uint16_t last_send_num = 0;
    String string = "";
    send_timer = _undownloadByte / 4096 + 1;
    last_send_num = _undownloadByte % 4096;

    while (send_timer)
    {

        if (send_timer == 1)
        {
            for (uint16_t j = 1; j <= 4096; j++)
            {
                if (j <= last_send_num)
                {
                    readchar = asset.read(thechar, 1);
                    if (readchar == 1)
                        Serial1.write(thechar[0]);
                    else
                        return false;
                }
                else
                    break;
            }
        }

        else
        {
            for (uint16_t i = 1; i <= 4096; i++)
            {
                readchar = asset.read(thechar, 1);
                if (readchar == 1)
                    Serial1.write(thechar[0]);
                else
                    return false;
            }
        }

        this->recvRetString(string, 500, true);
        if (string.indexOf(0x05) != -1)
        {
            string = "";
        }
        else
        {
            return 0;
        }
        --send_timer;
    }

    return 1;
}
