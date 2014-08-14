package atlantis.projection;

import atlantis.graphics.ACoord;
import atlantis.graphics.colormap.AColorMap;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AParameter;
import atlantis.parameters.APar;
import atlantis.utils.AMath;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.geom.Point2D;

import atlantis.canvas.AWindow;
import atlantis.data.ADHelix;

public class AProjectionVP extends AProjectionPhi {

  // this should be moved ugly
  // sign is which arm of the v is being drawn at this point in time
  public static int sign;

  private static double qcp=0.00035936;

  // Display modes as defined in the configuration file.
  public static final int MODE_STANDARD = 0;
  public static final int MODE_SPACEPOINT = 1;
  public static final int MODE_TRT_DRIFT_CIRCLE = 2;
  public static final int MODE_ECAL_LAYER_0 = 3;
  public static final int MODE_ECAL_LAYER_1 = 4;
  public static final int MODE_ECAL_LAYER_2 = 5;
  public static final int MODE_ECAL_LAYER_3 = 6;
  public static final int MODE_HCAL_LAYER_0 = 7;
  public static final int MODE_HCAL_LAYER_1 = 8;
  public static final int MODE_HCAL_LAYER_2 = 9;
  public static final int MODE_HCAL_LAYER_3 = 10;
  
  public static final double MAX_ETA = 5.;

  private static boolean first=true;
  // draw V apex at this rho or Z
  private static double[] rhoMode=new double[]
      { 55., 55., 108., 138., 154., 173., 193., 244., 306., 368., 368.};
  private static double[] zMode=new double[]
      {277., 277., 338., 360., 374., 394., 414., 441., 482., 561., 561.};
  // zoom in another projection to this rho and Z
  private static double[] rhoZoomMode=new double[]
      { 56., 56., 110., 200., 200., 200., 200., 400., 400., 400., 400.};
  private static double[] zZoomMode=new double[]
      {280., 280., 340., 440., 440., 440., 440., 374., 374., 374., 374.};

  private static AParameter modePar;
  private static AParameter rMaxPar;
  private static AParameter zMaxPar;
  private static AParameter gradientPar;
  private static AParameter zVtxPar;

  public AProjectionVP() {}

  public String getName() {
    return "VP";
  }

  public String getXLabel() {
    return AMath.ETA;
  }

  public String getXUnits() {
    return "";
  }

  public void setAspectRatio1(AWindow window)
  {
      AOutput.append("\nNot implemented for this projection\n", ALogInterface.WARNING);
  }
  
  public Point2D.Double[] calculateNoZoomCorners(Dimension wSize) {
    Point2D.Double[] corners=new Point2D.Double[3];

    corners[0]=new Point2D.Double(-5.0, 360);
    corners[1]=new Point2D.Double(5.0, 360);
    corners[2]=new Point2D.Double(5.0, 0);
    return corners;
  }

  protected Color getBackgroundFillColor(Color[] colorMap) {
    int mode=parameterStore.get("VP", "Mode").getI();

    switch(mode) {
        case MODE_SPACEPOINT:
            return colorMap[parameterStore.get("Det", "SIL"+"Fill").getI()];
        case MODE_TRT_DRIFT_CIRCLE:
            return colorMap[parameterStore.get("Det", "TRT"+"Fill").getI()];
        case MODE_ECAL_LAYER_0:
        case MODE_ECAL_LAYER_1:
        case MODE_ECAL_LAYER_2:
        case MODE_ECAL_LAYER_3:
            return colorMap[parameterStore.get("Det", "ECAL"+"Fill").getI()];
        case MODE_HCAL_LAYER_0:
        case MODE_HCAL_LAYER_1:
        case MODE_HCAL_LAYER_2:
        case MODE_HCAL_LAYER_3:
            return colorMap[parameterStore.get("Det", "HCAL"+"Fill").getI()];
        default:
            return colorMap[parameterStore.get("Color", "Bkg"+"Fill").getI()];
    }
  }

  public ACoord getUserPoint(ADHelix dH, double s) {
    return dH.getVPPoint(s, this.sign);
  }

  public static double getDeltaEta(double rho, double z) {
    if(first) {
      modePar=parameterStore.get("VP", "Mode");
      rMaxPar=parameterStore.get("VP", "RMax");
      zMaxPar=parameterStore.get("VP", "ZMax");
      gradientPar=parameterStore.get("VP", "Gradient");
      zVtxPar=parameterStore.get("VP", "ZVtx");
      first=false;
    }
    double fact=qcp*gradientPar.getD();
    int mode=modePar.getI();
    double zLayer=zMode[mode];

    if(zMaxPar.getStatus())
      zLayer=Math.min(zLayer, zMaxPar.getD());
    double rhoLayer=rhoMode[mode];

    if(rMaxPar.getStatus())
      rhoLayer=Math.min(rhoLayer, rMaxPar.getD());
    double zVtx=zVtxPar.getD();
    double zsign=1.;

    if(z<0.) zsign=-1.;
    if(Math.abs((z-zVtx)/rho)<(zLayer-zVtx)/rhoLayer)
      return fact*(rhoLayer-rho);
    else
      return fact*rho*(zsign*zLayer-z)/(z-zVtx);
  }

  public static double getRhoVPlot() {
    double rhoLayer=rhoMode[parameterStore.get("VP", "Mode").getI()];
    AParameter rMaxPar=parameterStore.get("VP", "RMax");

    if(rMaxPar.getStatus())
      rhoLayer=Math.min(rhoLayer, rMaxPar.getD());
    return rhoLayer;
  }

  public static double getZVPlot() {
    double zLayer=zMode[parameterStore.get("VP", "Mode").getI()];
    AParameter zMaxPar=parameterStore.get("VP", "ZMax");

    if(zMaxPar.getStatus())
      zLayer=Math.min(zLayer, zMaxPar.getD());
    return zLayer;
  }

  public static double getRhoZoomVPlot() {
    return rhoZoomMode[parameterStore.get("VP", "Mode").getI()];
  }

  public static double getZZoomVPlot() {
    return zZoomMode[parameterStore.get("VP", "Mode").getI()];
  }

}
