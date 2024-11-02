#include "Particle.h"
#include "NexUpload.h"

SYSTEM_MODE(AUTOMATIC);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

void handleAssets(spark::Vector<ApplicationAsset> assets)
{

  waitFor(Serial.isConnected, 5000);
  delay(1000);

  for (ApplicationAsset &asset : assets)
  {
    Log.info("Found asset name=%s with size=%d", asset.name().c_str(), (int)asset.size());

    if (asset.name() == "nextion.tft")
    {

      Log.info("Nextion TFT file found, writing to Nextion");

      NexUpload nex_download("nex.tft", 10, 115200);
      nex_download.upload();
    }
  }

  System.assetsHandled(true);
}

STARTUP(System.onAssetOta(handleAssets));

void setup()
{
}

void loop()
{
}
