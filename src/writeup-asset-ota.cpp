#include "Particle.h"
#include "NexUpload.h"

SYSTEM_MODE(AUTOMATIC);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

void handleAssets(spark::Vector<ApplicationAsset> assets)
{
  for (ApplicationAsset &asset : assets)
  {
    if (asset.name() == "nextion.tft")
    {
      Log.info("Nextion TFT file found, writing to Nextion");
      NexUpload nex_download(1);
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
