package atlantis.data;


import atlantis.graphics.ACoord;


public interface AHistogramData {
  ACoord[] getYXHistograms();
  ACoord[] getRZHistograms();
  ACoord[] getFRHistograms();
  ACoord[] getFZHistograms();
  ACoord[] getXZHistograms();
  ACoord[] getYZHistograms();
  ACoord[] getVPHistograms();
}
