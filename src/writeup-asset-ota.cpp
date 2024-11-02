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
      NexUpload nextion(0);
      nextion.upload();
    }
  }

  System.assetsHandled(true);
}

STARTUP(System.onAssetOta(handleAssets));

void setup()
{
  // re-enalbe the asset when testing
  // System.assetsHandled(false);
}

void loop()
{
}
