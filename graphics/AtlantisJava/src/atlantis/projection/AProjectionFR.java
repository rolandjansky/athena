package atlantis.projection;

import java.awt.Dimension;
import java.awt.geom.Point2D;

import atlantis.canvas.ACanvas;
import atlantis.data.ADHelix;
import atlantis.graphics.ACoord;
import atlantis.interactions.AZMRInteraction;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.utils.AMath;

public class AProjectionFR extends AProjectionPhi
{
    public String getName()
    {
        return "FR";
    }

    public String getXLabel()
    {
        return AMath.RHO;
    }

    public String getXUnits()
    {
        return "(cm)";
    }

    public Point2D.Double[] calculateNoZoomCorners(Dimension wSize)
    {
        Point2D.Double[] corners = new Point2D.Double[3];
        int phiWrap = parameterStore.get(getName(), "PhiWrap").getI();

        double radius = parameterStore.get("Projection", "Radius").getD();
        corners[0] = new Point2D.Double(0, 360 + phiWrap);
        corners[1] = new Point2D.Double(radius, 360 + phiWrap);
        corners[2] = new Point2D.Double(radius, 0);
        return corners;
    }

    public ACoord getUserPoint(ADHelix dH, double s)
    {
        return nonLinearTransform(dH.getFRPoint(s));
    }

    public ACoord nonLinearTransform(ACoord user)
    {
        AParameter fishEyePar = parameterStore.get(getName(), "FishEye");

        // Fisheye of projection FR should not change phi
        if (fishEyePar.getStatus())
        {
            double[] r;
            //double[] phi;
            double fishEye = 0.001 * fishEyePar.getD();
            double rTo = parameterStore.get(getName(), "rTo").getD();
            //Point2D.Double[] corners = ACanvas.getCanvas().getCurrentWindow().getUserCorners();
            //double phiMid = (corners[0].getY() + corners[2].getY()) / 2.;

            for (int j = 0; j < user.hv[0].length; ++j)
            {
                r = user.hv[0][j];
                //phi = user.hv[1][j];
                for (int i = 0; i < r.length; ++i)
                {
                    double fact = (1 + fishEye * rTo) / (1 + fishEye * r[i]);

                    //phi[i] = phiMid + (phi[i] - phiMid) * fact;
                    r[i] *= fact;
                }
            }
        }
        
       // Horizontal Zoom is applied in ZMR interaction -
       // don't know what this has to do here?
       // CLEANUP - S.B.
   /*     AParameter horZoomPar = APar.get(getName(), "HorizontalZoom");

        if (horZoomPar.getStatus())
        {
            // performe initial horizontal zoom
            double hzf = APar.get("FR", "HorizontalZoom").getD();
            Point2D.Double[] corners = ACanvas.getCanvas().getCurrentWindow().getUserCorners();
            double centerRho = corners[0].getX();
            double centerPhi = (corners[0].getY() + corners[2].getY()) / 2.;
            Point2D.Double center = new Point2D.Double(centerRho, centerPhi);
            AZMRInteraction.performHorizontalZoom(center, hzf, ACanvas.getCanvas().getPaintingWindow());
            
            horZoomPar.setStatus(false);
        }
    */    
        return user;
    }

    public ACoord inverseNonLinearTransform(ACoord user)
    {
        AParameter fishEyePar = parameterStore.get(getName(), "FishEye");

        if (fishEyePar.getStatus())
        {
            double[] r;
            //double[] phi;
            double fishEye = 0.001 * fishEyePar.getD();
            double rTo = parameterStore.get(getName(), "rTo").getD();
            //Point2D.Double[] corners = ACanvas.getCanvas().getCurrentWindow().getUserCorners();
            //double phiMid = (corners[0].getY() + corners[2].getY()) / 2.;

            for (int j = 0; j < user.hv[0].length; ++j)
            {
                r = user.hv[0][j];
                //phi = user.hv[1][j];
                for (int i = 0; i < r.length; ++i)
                {
                    double fact = 1 / (1 + fishEye * (rTo - r[i]));

                    //phi[i] = phiMid + (phi[i] - phiMid) * fact;
                    r[i] *= fact;
                }
            }
        }

        return user;
    }

}
