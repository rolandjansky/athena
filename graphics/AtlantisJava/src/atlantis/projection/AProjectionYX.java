package atlantis.projection;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Point2D;

import javax.swing.ImageIcon;

import atlantis.canvas.AWindow;
import atlantis.data.ADHelix;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.ADrawParameters;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.interactions.AZMRInteraction;
import atlantis.parameters.AParameter;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;

public class AProjectionYX extends AProjection2D
{
    private static ALogger logger = ALogger.getLogger(AProjectionYX.class);
    
    // Different display modes, as defined in the configuration file.
    public static final int MODE_STANDARD = 0;
    public static final int MODE_TGC_INNER = 1;
    public static final int MODE_TGC_MIDDLE_1 = 2;
    public static final int MODE_TGC_MIDDLE_2 = 3;
    public static final int MODE_TGC_MIDDLE_3 = 4;
    public static final int MODE_MDT_INNER = 5;
    public static final int MODE_MDT_EXTENSION = 6;
    public static final int MODE_MDT_MIDDLE = 7;
    public static final int MODE_MDT_OUTER = 8;
    public static final int MODE_FCAL_EM = 9;
    public static final int MODE_FCAL_HAD_1 = 10;
    public static final int MODE_FCAL_HAD_2 = 11;
    public static final int MODE_LAR_ENDCAP_PRESAMPLER = 12;
    public static final int MODE_LAR_ENDCAP_1 = 13;
    public static final int MODE_LAR_ENDCAP_2 = 14;
    public static final int MODE_LAR_ENDCAP_3 = 15;
    public static final int MODE_HEC_1 = 16;
    public static final int MODE_HEC_2 = 17;
    public static final int MODE_HEC_3 = 18;
    public static final int MODE_HEC_4 = 19;
    public static final int MODE_LAR_ENDCAP_SUMMED = 20;
    public static final int MODE_HEC_SUMMED = 21;
    public static final int MODE_MBTS = 22;

    public AProjectionYX()
    {
        addPopupItem(ASPECT_RATIO_1);
    }

    public String getName()
    {
        return "YX";
    }

    public String getXLabel()
    {
        return "X";
    }

    public String getYLabel()
    {
      return "Y";
    }

    public String getXUnits()
    {
      return "(cm)";
    }

    public String getYUnits()
    {
      return "(cm)";
    }

    public Point2D.Double getCenter()
    {
      return new Point2D.Double(0, 0);
    }

    public Point2D.Double[] calculateNoZoomCorners(Dimension wSize)
    {
      // should not be here
      parameterStore.get(getName(), "Phi").setD(0.);
      double radius = parameterStore.get("Projection", "Radius").getD();
      return aspectRatioLayout(radius, radius, wSize);
    }

    public static double[][] getRotationMatrix()
    {
      double[][] r = new double[3][3];

      r[0][0] = 1.;
      r[1][1] = 1.;
      r[2][2] = 1.;
      return r;
    }

    public static int[] getAxisMapping()
    {
      return new int[] { 0, 1, 2 };
    }

    public ACoord getUserPoint(ADHelix dH, double s)
    {
      return nonLinearTransform(dH.getYXPoint(s));
    }

    public ACoord nonLinearTransform(ACoord user)
    {
      AParameter fishEyePar = parameterStore.get(getName(), "FishEye");

      if (fishEyePar.getStatus())
      {
        double[] x, y;
        double r, fact;
        double fishEye = 0.001 * fishEyePar.getD();
        double rTo = parameterStore.get(getName(), "rTo").getD();

        for (int j = 0; j < user.hv[0].length; ++j)
        {
          x = user.hv[0][j];
          y = user.hv[1][j];
          for (int i = 0; i < x.length; ++i)
          {
            r = Math.sqrt(x[i] * x[i] + y[i] * y[i]);
            fact = (1 + fishEye * rTo) / (1 + fishEye * r);
            x[i] *= fact;
            y[i] *= fact;
          }
        }
      }
      AParameter clockPar = parameterStore.get(getName(), "Clock");

      if (clockPar.getStatus())
      {
        double clock = Math.PI / clockPar.getD();
        double phiClock = Math.toRadians(parameterStore.get(getName(), "Phi").getD());

        for (int j = 0; j < user.hv[0].length; ++j)
        {
          double[] x = user.hv[0][j];
          double[] y = user.hv[1][j];

          for (int i = 0; i < x.length; ++i)
          {
            double phi = Math.atan2(y[i], x[i]);

            if (phi < 0.)
              phi += 2 * Math.PI;
            double rho = Math.sqrt(x[i] * x[i] + y[i] * y[i]);
            double dPhi = phi - phiClock;

            if (dPhi > Math.PI)
              dPhi -= 2 * Math.PI;
            else if (dPhi <= -Math.PI)
              dPhi += 2 * Math.PI;
            double fact = (clock + Math.PI) / (clock + Math.abs(dPhi));

            phi = phiClock + fact * dPhi;
            x[i] = rho * Math.cos(phi);
            y[i] = rho * Math.sin(phi);
          }
        }
      }

      return user;
    }

    public ACoord inverseNonLinearTransform(ACoord user)
    {
      AParameter fishEyePar = parameterStore.get(getName(), "FishEye");

      if (fishEyePar.getStatus())
      {
        double[] x, y;
        double r, fact;
        double fishEye = 0.001 * fishEyePar.getD();
        double rTo = parameterStore.get(getName(), "rTo").getD();

        for (int j = 0; j < user.hv[0].length; ++j)
        {
          x = user.hv[0][j];
          y = user.hv[1][j];
          for (int i = 0; i < x.length; ++i)
          {
            r = Math.sqrt(x[i] * x[i] + y[i] * y[i]);
            fact = 1 / (1 + fishEye * (rTo - r));
            x[i] *= fact;
            y[i] *= fact;
          }
        }
      }
      AParameter clockPar = parameterStore.get(getName(), "Clock");

      if (clockPar.getStatus())
      {
        double clock = Math.PI / clockPar.getD();
        double phiClock = Math.toRadians(parameterStore.get(getName(), "Phi").getD());

        for (int j = 0; j < user.hv[0].length; ++j)
        {
          double[] x = user.hv[0][j];
          double[] y = user.hv[1][j];

          for (int i = 0; i < x.length; ++i)
          {
            double phi = Math.atan2(y[i], x[i]);

            if (phi < 0.)
              phi += 2 * Math.PI;
            double dPhi = phi - phiClock;

            if (dPhi > Math.PI)
              dPhi -= 2 * Math.PI;
            else if (dPhi <= -Math.PI)
              dPhi += 2 * Math.PI;

            double dPhiPrime = (clock * dPhi) / (clock + Math.PI - Math.abs(dPhi));

            phi = phiClock + dPhiPrime;

            double rho = Math.sqrt(x[i] * x[i] + y[i] * y[i]);

            x[i] = rho * Math.cos(phi);
            y[i] = rho * Math.sin(phi);
          }
        }
      }
      return user;
    }

    public void paint(AWindow window, Graphics g)
    {
      super.paint(window, g);

      // draw RZ cutting plane
        if (parameterStore.get("YX", "RZCuttingPlane").getStatus())
        {
            AGraphics ag = AGraphics.makeAGraphics(g);
            final ADrawParameters drawParameters = 
                new ADrawParameters(true, AColorMap.WH, 4, 1, 0, AGraphics.SYMBOL_FILLED_BOX);
            ag.updateDrawParameters(drawParameters);
            final double RHO_MAX = 3000.0;

            double phi = Math.toRadians(parameterStore.get("RZ", "Phi").getD()) - Math.PI / 2;
            double cosPhi = Math.cos(phi);
            double sinPhi = Math.sin(phi);
            String iconPath = AGlobals.instance().getHomeDirectory() + "img" + System.getProperty("file.separator");
            
            // draw the cutting plane
            Point2D.Double p1 = window.calculateDisplay(nonLinearTransform(RHO_MAX * cosPhi, RHO_MAX * sinPhi));
            Point2D.Double p2 = window.calculateDisplay(nonLinearTransform(-RHO_MAX * cosPhi, -RHO_MAX * sinPhi));

            ag.drawLine(p1.x, p1.y, p2.x, p2.y);

            // draw the arrows
            Point2D.Double p3 = window.calculateDisplay(nonLinearTransform(1500 * Math.cos(phi+.07), 1500 * Math.sin(phi+.07)));
            Point2D.Double p4 = window.calculateDisplay(nonLinearTransform(1500 * Math.cos(phi+3.21), 1500 * Math.sin(phi+3.21)));

            ImageIcon upArrow = AUtilities.getFileAsImageIcon(iconPath+"uparrow.png");
            ImageIcon downArrow = AUtilities.getFileAsImageIcon(iconPath+"downarrow.png");

            Graphics2D g2d = (Graphics2D) g;

            g2d.drawImage(upArrow.getImage(),(int) p3.x,(int) p3.y,null);
            g2d.drawImage(downArrow.getImage(),(int) p4.x,(int) p4.y,null);

        }
    }

    /** This function ensures that when the aspect ratio of the display changes
     * the aspect ratio of the contents of a YX projection remain at the same
     * aspect ratio.
     * 
     * It uses the (thankfully) static methods of AZMRInteraction to do the
     * actual zooming, which can involve some non-trivial mathematics...
     *  -- Adam Davison
     */
    @Override
    public void aspectRatioChange(AWindow w, double oldaspect, double newaspect) {
        logger.debug("ProjectionYX informed of aspect ratio change from " + oldaspect + " to " + newaspect);
        
        Point2D.Double[] corners = w.getUserCorners();
        
        // Get center
        Point2D.Double center = new Point2D.Double(
                0.5*(corners[0].x+corners[2].x),
                0.5*(corners[0].y+corners[2].y));
        
        // If > 1, the new view is taller if < 1 it's wider
        double fracchange = newaspect/oldaspect;
        
        if (fracchange > 1) {
            // Need to vzoom "away" to restore aspect ratio
            AZMRInteraction.performVerticalZoom(center, fracchange, w);
        } else {
            // Need to hzoom
            AZMRInteraction.performHorizontalZoom(center, 1.0/fracchange, w);
        }
    }
}
