package atlantis.geometry;

import atlantis.utils.*;
import atlantis.graphics.ACoord;

public class ADiscDetector extends ADetector {

  double rInner;
  double rOuter;
  int nInner;
  int nOuter;

  ADiscDetector(String name, String projection, String color, double rInner, double rOuter, int nInner, int nOuter) {
    super(name, projection, color);
    this.rInner=rInner;
    this.nInner=nInner;
    this.rOuter=rOuter;
    this.nOuter=nOuter;
  }

  protected ACoord getYXUser() {
      if (!projection.equals("YX")) {
          return ACoord.NO_DATA;
      }

      if (!globals.getUseOpenGL()) {
          int numPoints = nInner + nOuter + 2;
          double[][] hv = new double[2][numPoints];

          for (int j = 0; j < nInner + 1; j++) {
              hv[0][j] = rInner * Math.cos(Math.PI * 2 * j / nInner);
              hv[1][j] = rInner * Math.sin(Math.PI * 2 * j / nInner);
          }
          for (int j = 0; j < nOuter + 1; j++) {
              hv[0][j + nInner + 1] = rOuter * Math.cos(Math.PI * 2 * (nOuter - j) / nOuter);
              hv[1][j + nInner + 1] = rOuter * Math.sin(Math.PI * 2 * (nOuter - j) / nOuter);
          }
          return new ACoord(hv);
      } else {
          //FIXME:
          //Note that generateAnnulusTris can't really handle this different number
          //of inner and outer points thing so we're just going to take the mean here
          //Maybe one day we fix this somehow or just remove one of the ns
          //All the other round detectors get along fine without it...
          // -- Adam
          return new ACoord(generateAnnulusTris((nInner + nOuter) / 2, 0.0, rInner, rOuter));
      }
  }

  protected ACoord getFRUser() {
    if(!projection.equals("FR")) return ACoord.NO_DATA;
    return new ACoord (AMath.xBox((rInner+rOuter)/2.,(rOuter-rInner)/2.),
                            AMath.yBox(180.,180.));
  }

}

