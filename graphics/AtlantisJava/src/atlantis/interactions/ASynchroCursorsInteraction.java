package atlantis.interactions;

import java.awt.Graphics2D;
import java.awt.event.KeyEvent;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Point2D;

import atlantis.canvas.ACanvas;
import atlantis.graphics.ACoord;
import atlantis.graphics.ACursorFactory;
import atlantis.graphics.AGraphics;
import atlantis.parameters.APar;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionXZ;
import atlantis.projection.AProjectionYX;
import atlantis.projection.AProjectionYZ;
import atlantis.utils.AMath;

public class ASynchroCursorsInteraction extends AInteraction
        implements ASleepMouseDragListener, AEnterExitListener
{
    /**
     * The position of the moving marker in 3D. Phi is in degrees.
     * (Must be static)
     */
    private static double rho, phi, z, sign_rho;

    private static final double NOT_DEFINED = 9999.0;
    private static final double RHO_MAX = 3000.0;
    private static final double PHI_MAX = 400.0;
    private static final double Z_MAX = 3000.0;
    private static final double ETA_MAX = 10.0;

    private static int key = 0;
    protected int radius = 10;

    public ASynchroCursorsInteraction()
    {
        super(1, ALL_WINDOWS_REPAINT, WORLD_COORDINATES, true);
        hr[0] = new Ellipse2D.Double(0, 0, radius, radius);
    }

    public int getButton()
    {
        return AInteractionsManager.LEFT;
    }

    /**
     * Initialize the control points based on the starting point
     */
    public int init(Point2D.Double p, int key)
    {
        this.key = key;
        setCenter(hr[0], p.x, p.y);
        // ACanvas.getCanvas().repaintOthers(window);
        return 0;
    }

    public void start(Point2D.Double p, int region, int key)
    {
      //call drag to update the cursor even if only a button is pressed
      drag(p,region,key);
    }

    /**
     * Move the active control point to the point (x, y).
     *
     * @param pInput the input point
     * @param region the region
     * @param key the current key being pressed
     */
    public void drag(Point2D.Double pInput, int region, int key)
    {
        this.key = key;
        AProjection2D projection = (AProjection2D) window.getProjection();

        Point2D.Double p = projection.inverseNonLinearTransform(pInput);

        setCenter(hr[0], p.x, p.y);

        rho = NOT_DEFINED;
        z = NOT_DEFINED;
        phi = NOT_DEFINED;
        sign_rho = 1.;

        if (projection instanceof AProjectionYX)
        {
            phi = Math.toDegrees(Math.atan2(p.y, p.x));
            if (phi < 0.)
                phi += 360.;
            rho = AParameterUtilities.getRhoSign(p.x, p.y) * Math.sqrt(p.x * p.x + p.y * p.y);
        }
        else if (projection instanceof AProjectionFR)
        {
            phi = p.y;
            rho = p.x;
        }
        else if (projection instanceof AProjectionFZ)
        {
            phi = p.y;
            z = p.x;
        }
        else if (projection instanceof AProjectionRZ)
        {
            rho = Math.abs(p.y);
            if (p.y < 0.)
                sign_rho = -1.;
            z = p.x;
        }
        else if (projection instanceof AProjectionYZ)
        {
            z = p.x;
        }
        else if (projection instanceof AProjectionXZ)
        {
            z = p.x;
        }
        else if (projection instanceof AProjectionVP)
        {
            phi = p.y;
            rho = parameterStore.get("VP", "RMax").getD();
            double eta = p.x;

            z = parameterStore.get("Event", "ZVtx").getD() + 0.5 * rho * (Math.exp(eta) - 1. / Math.exp(eta));
        }
        else if (projection instanceof AProjectionLegoPlot)
        {
            // added by Mark Stockton
            // update parameters
            AProjectionLegoPlot.update();
            //reverse the adjustPhi function
            p.x=-(AProjectionLegoPlot.adjustPhi(window,-p.x,p.y));
            if(p.x>-1.0 && p.x<361.0 && p.y>-5.01 && p.y<5.01)
            {
                phi=p.x;
                rho = parameterStore.get("VP", "RMax").getD();
                double eta=p.y;
                if (AProjectionLegoPlot.reverse)
                    eta = -eta;
                z = parameterStore.get("Event", "ZVtx").getD() + 0.5 * rho * (Math.exp(eta) - 1. / Math.exp(eta));
            }
        }
        if (key == KeyEvent.VK_P && phi != NOT_DEFINED)
        {
            double phiRZ = phi + 90.;

            if (phiRZ < 0.)
                phiRZ += 360;
            if (phiRZ > 360.)
                phiRZ -= 360;
            parameterStore.get("RZ", "Phi").setD(phiRZ);
            parameterStore.get("RZ", "Phi").globalize(ACanvas.getCanvas().getCurrentWindow().getIndex());
            ACanvas.getCanvas().repaintAllFromScratch();
        }
        else
            ACanvas.getCanvas().repaintOthers(window);
    }

    public void cancel()
    {}

    public void stop()
    {}

    public void paint(Graphics2D g)
    {
        // must use the window version of the parameters
        parameterStore.selectWindowParameters(window.getName());
        AProjection2D projection = (AProjection2D) window.getProjection();
        AGraphics ag = AGraphics.makeAGraphics(g);

        for (int i = 0; i < 2; i++)
        {
            if (i == 0)
                ag.updateDrawParameters(frameDrawParameters);
            else
                ag.updateDrawParameters(drawParameters);

            if (projection instanceof AProjectionYX)
            {
                if (phi != NOT_DEFINED)
                {
                    double r = (rho == NOT_DEFINED) ? RHO_MAX : Math.abs(rho);
                    double cosPhi = Math.cos(Math.toRadians(phi));
                    double sinPhi = Math.sin(Math.toRadians(phi));

                    Point2D.Double p = window.calculateDisplay(projection.nonLinearTransform(0, 0));
                    Point2D.Double p1 = window.calculateDisplay(projection.nonLinearTransform(r * cosPhi, r * sinPhi));
                    Point2D.Double p2 = window.calculateDisplay(projection.nonLinearTransform(RHO_MAX * cosPhi, RHO_MAX * sinPhi));
                    Point2D.Double p3 = window.calculateDisplay(projection.nonLinearTransform(-RHO_MAX * cosPhi, -RHO_MAX * sinPhi));

                    if (key == KeyEvent.VK_P)
                    {
                        ag.drawLine(p2.x, p2.y, p3.x, p3.y);
                    }
                    else
                    {
                        ag.drawLine(p.x, p.y, p2.x, p2.y);
                        if (rho != NOT_DEFINED)
                            ag.drawSymbol(p1.x, p1.y);
                    }

                }
                else if (rho != NOT_DEFINED)
                {
                    int numPoints = 48;
                    double[][][] hv = new double[2][1][numPoints];

                    for (int j = 0; j < numPoints; j++)
                    {
                        hv[0][0][j] = rho * Math.cos(Math.PI * 2 * j / 48);
                        hv[1][0][j] = rho * Math.sin(Math.PI * 2 * j / 48);
                    }
                    ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                    ag.drawPolygon(d.hv[0][0], d.hv[1][0], numPoints);
                }
            }
            else if (projection instanceof AProjectionFR)
            {
                int numPoints = 5;
                double[][][] hv = new double[2][1][numPoints];

                hv[0][0][0] = Math.abs(rho);
                hv[1][0][0] = phi;
                hv[0][0][1] = Math.abs(rho);
                hv[1][0][1] = 0.;
                hv[0][0][2] = Math.abs(rho);
                hv[1][0][2] = RHO_MAX;
                hv[0][0][3] = 0.;
                hv[1][0][3] = phi;
                hv[0][0][4] = RHO_MAX;
                hv[1][0][4] = phi;
                ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                if (rho != NOT_DEFINED)
                    ag.drawLine(d.hv[0][0][1], d.hv[1][0][1], d.hv[0][0][2], d.hv[1][0][2]);
                if (phi != NOT_DEFINED)
                    ag.drawLine(d.hv[0][0][3], d.hv[1][0][3], d.hv[0][0][4], d.hv[1][0][4]);
                if (rho != NOT_DEFINED && phi != NOT_DEFINED)
                    ag.drawSymbol(d.hv[0][0][0], d.hv[1][0][0]);
            }
            else if (projection instanceof AProjectionFZ)
            {
                int numPoints = 5;
                double[][][] hv = new double[2][1][numPoints];

                hv[0][0][0] = z;
                hv[1][0][0] = phi;
                hv[0][0][1] = z;
                hv[1][0][1] = 0.;
                hv[0][0][2] = z;
                hv[1][0][2] = PHI_MAX;
                hv[0][0][3] = -Z_MAX;
                hv[1][0][3] = phi;
                hv[0][0][4] = Z_MAX;
                hv[1][0][4] = phi;
                ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                if (z != NOT_DEFINED)
                    ag.drawLine(d.hv[0][0][1], d.hv[1][0][1], d.hv[0][0][2], d.hv[1][0][2]);
                if (phi != NOT_DEFINED)
                    ag.drawLine(d.hv[0][0][3], d.hv[1][0][3], d.hv[0][0][4], d.hv[1][0][4]);
                if (z != NOT_DEFINED && phi != NOT_DEFINED)
                    ag.drawSymbol(d.hv[0][0][0], d.hv[1][0][0]);
            }
            else if (projection instanceof AProjectionRZ)
            {
                if (rho != NOT_DEFINED && phi != NOT_DEFINED && z != NOT_DEFINED && (rho != 0. || z != 0.))
                {
                    int numPoints = 15;
                    double[][][] hv = new double[2][1][numPoints];

                    double dr, dz;
                    if (Math.abs(rho/RHO_MAX) > Math.abs(z/Z_MAX)) {
                        dr = RHO_MAX / (numPoints-1);
                        dz = z * (RHO_MAX / ((numPoints-1)*rho));
                    } else {
                        dz = AMath.getSign(z) * Z_MAX / (numPoints-1);
                        dr = rho * Math.abs(Z_MAX / ((numPoints-1)*z));
                    }
                    
                    double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());

                    if (phiMid > AMath.TWO_PI)
                        phiMid -= AMath.TWO_PI;
                    double phiDiff = Math.abs(Math.toRadians(phi) - phiMid);

                    if (phiDiff > Math.PI / 2. && phiDiff < 3 * Math.PI / 2.)
                        dr *= -1;

                    for (int j=0; j<numPoints; j++) {
                        hv[0][0][j] = j*dz;
                        hv[1][0][j] = j*dr;
                    }

                    ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                    ag.drawSmoothPolyline(d.hv[0][0], d.hv[1][0], numPoints);
                }
                else
                {
                    int numPoints = 5;
                    double[][][] hv = new double[2][1][numPoints];

                    hv[0][0][0] = z;
                    hv[1][0][0] = rho * sign_rho;
                    hv[0][0][1] = z;
                    hv[1][0][1] = -RHO_MAX;
                    hv[0][0][2] = z;
                    hv[1][0][2] = RHO_MAX;
                    hv[0][0][3] = -Z_MAX;
                    hv[1][0][3] = rho * sign_rho;
                    hv[0][0][4] = Z_MAX;
                    hv[1][0][4] = rho * sign_rho;
                    ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                    if (z != NOT_DEFINED)
                        ag.drawLine(d.hv[0][0][1], d.hv[1][0][1], d.hv[0][0][2], d.hv[1][0][2]);
                    if (rho != NOT_DEFINED)
                        ag.drawLine(d.hv[0][0][3], d.hv[1][0][3], d.hv[0][0][4], d.hv[1][0][4]);
                    if (z != NOT_DEFINED && rho != NOT_DEFINED)
                        ag.drawSymbol(d.hv[0][0][0], d.hv[1][0][0]);
                }
            }
            else if (projection instanceof AProjectionYZ)
            {
                if (rho != NOT_DEFINED && phi != NOT_DEFINED && z != NOT_DEFINED)
                {
                    int numPoints = 2;
                    double[][][] hv = new double[2][1][numPoints];

                    hv[0][0][0] = 0.;
                    hv[1][0][0] = 0.;
                    double phiRot = AProjectionXZ.getPhi();

                    hv[0][0][1] = z;
                    hv[1][0][1] = rho * Math.sin(phiRot);

                    ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                    ag.drawLine(d.hv[0][0][0], d.hv[1][0][0], d.hv[0][0][1], d.hv[1][0][1]);
                    ag.drawSymbol(d.hv[0][0][1], d.hv[1][0][1]);
                }
                else if (z != NOT_DEFINED)
                {
                    int numPoints = 2;
                    double[][][] hv = new double[2][1][numPoints];

                    hv[0][0][0] = z;
                    hv[1][0][0] = -RHO_MAX;
                    hv[0][0][1] = z;
                    hv[1][0][1] = RHO_MAX;
                    ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                    ag.drawLine(d.hv[0][0][0], d.hv[1][0][0], d.hv[0][0][1], d.hv[1][0][1]);
                }
            }
            else if (projection instanceof AProjectionXZ)
            {
                if (rho != NOT_DEFINED && phi != NOT_DEFINED && z != NOT_DEFINED)
                {
                    int numPoints = 2;
                    double[][][] hv = new double[2][1][numPoints];

                    hv[0][0][0] = 0.;
                    hv[1][0][0] = 0.;
                    double phiRot = AProjectionXZ.getPhi();

                    hv[0][0][1] = z;
                    hv[1][0][1] = rho * Math.cos(phiRot);

                    ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                    ag.drawLine(d.hv[0][0][0], d.hv[1][0][0], d.hv[0][0][1], d.hv[1][0][1]);
                    ag.drawSymbol(d.hv[0][0][1], d.hv[1][0][1]);
                }
                else if (z != NOT_DEFINED)
                {
                    int numPoints = 2;
                    double[][][] hv = new double[2][1][numPoints];

                    hv[0][0][0] = z;
                    hv[1][0][0] = -RHO_MAX;
                    hv[0][0][1] = z;
                    hv[1][0][1] = RHO_MAX;
                    ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                    ag.drawLine(d.hv[0][0][0], d.hv[1][0][0], d.hv[0][0][1], d.hv[1][0][1]);
                }
            }
            else if (projection instanceof AProjectionVP)
            {
                int numPoints = 5;
                double[][][] hv = new double[2][1][numPoints];
                double eta = 1.;

                if (z != NOT_DEFINED && rho != NOT_DEFINED)
                    eta = AParameterUtilities.eta(z, rho);
                hv[0][0][0] = eta;
                hv[1][0][0] = phi;
                hv[0][0][1] = eta;
                hv[1][0][1] = 0.;
                hv[0][0][2] = eta;
                hv[1][0][2] = PHI_MAX;
                hv[0][0][3] = -ETA_MAX;
                hv[1][0][3] = phi;
                hv[0][0][4] = ETA_MAX;
                hv[1][0][4] = phi;
                ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));

                if (z != NOT_DEFINED && rho != NOT_DEFINED)
                    ag.drawLine(d.hv[0][0][1], d.hv[1][0][1], d.hv[0][0][2], d.hv[1][0][2]);
                if (phi != NOT_DEFINED)
                    ag.drawLine(d.hv[0][0][3], d.hv[1][0][3], d.hv[0][0][4], d.hv[1][0][4]);

                if (z != NOT_DEFINED && phi != NOT_DEFINED && rho != NOT_DEFINED)
                    ag.drawSymbol(d.hv[0][0][0], d.hv[1][0][0]);
            }
            else if (projection instanceof AProjectionLegoPlot)
            {
                //added by Mark Stockton
                double eta = 1.;
                double[][][] hv = new double[2][1][3];
                Point2D.Double[] userCorners = window.getUserCorners();
                Point2D.Double[] displayCorners = window.getDisplayCorners();
                //update parameters
                AProjectionLegoPlot.update();
                if (z != NOT_DEFINED && rho != NOT_DEFINED)
                    eta = AParameterUtilities.eta(z, rho);
                if (AProjectionLegoPlot.reverse)
                    eta = -eta;
                hv[0][0][0] = AProjectionLegoPlot.adjustPhi(window,phi,eta);
                hv[1][0][0] = eta;
                //top of plot coordinate
                hv[0][0][1] = AProjectionLegoPlot.adjustPhi(window,phi,userCorners[0].y);
                hv[1][0][1] = userCorners[0].y;
                //bottom of plot coordinate
                hv[0][0][2] = AProjectionLegoPlot.adjustPhi(window,phi,userCorners[2].y);
                hv[1][0][2] = userCorners[2].y;
                //convert to screen positions
                ACoord d = window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv)));
                if (z != NOT_DEFINED && rho != NOT_DEFINED)
                    ag.drawLine(0, d.hv[1][0][0], displayCorners[1].x, d.hv[1][0][0]);//eta line
                if (phi != NOT_DEFINED)
                    ag.drawLine(d.hv[0][0][1], d.hv[1][0][1],d.hv[0][0][2], d.hv[1][0][2]);//phi line
                if (z != NOT_DEFINED && phi != NOT_DEFINED && rho != NOT_DEFINED)
                    ag.drawSymbol(d.hv[0][0][0], d.hv[1][0][0]);//square where axis cross
            }
        }
        parameterStore.restoreWindowParameters();
    }

    /**
     * Gets called each time we enter a window in which this interaction is active
     */
    public void entered()
    {
        //Set mouse cursor
        window.setCursor(ACursorFactory.getInstance().getSyncCursor());
    }

    /**
     * Gets called each time we leave a window in which this interaction is active
     */
    public void exited()
    {
       //Set mouse cursor
       window.setCursor(ACursorFactory.getInstance().getSyncCursor());
    }

    public AModifier[] getModifiers()
    {
        return new AModifier[] { new AModifier(KeyEvent.VK_P, false, "Copy phi to RZ projection" ) };
    }

}
