package atlantis.data;

import atlantis.event.AData;
import atlantis.canvas.AWindow;
import atlantis.event.AEvent;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

import java.awt.Stroke;
import java.awt.BasicStroke;
import java.awt.geom.Point2D;

/**
 * AETMisData - Missing ET class implementation. Although it is always
 * supposed that there will be only 1 item of the datatype, it's now done
 * generically as a normal datatype (assuming there is numData items, this
 * numData shall always be 1 for ETMis)
 */
public class AETMisData extends AData {
    // data read in from the event file

    protected float[] et;
    protected float[] etx;
    protected float[] ety;
    // calculated data
    protected float[] sumet;
    protected float[] phi;    // in radians
    protected final int YX_NUMPOINTS = 200;

    AETMisData(AHashMap p, AEvent e) throws AAtlantisException {
        super(p, e);

        sumet = p.getFloatArray("et");
        etx = p.getFloatArray("etx");
        ety = p.getFloatArray("ety");

        et = new float[numData];
        phi = new float[numData];
        for (int i = 0; i < numData; i++) {
            et[i] = (float) Math.sqrt(etx[i] * etx[i] + ety[i] * ety[i]);

            phi[i] = (float) Math.atan2(ety[i], etx[i]);
            if (phi[i] < 0.0) {
                phi[i] += AMath.TWO_PI;
            }
        }

    } // AETMisData() -------------------------------------------------------

    /**
     * Assuming there is only one item in the ETMis datatype which shall
     * always be, but the rest of the class is more generic (the 0 index)
     * @return float
     */
    public float getET() {
        return et[0];

    } // getET() ------------------------------------------------------------

    /**
     * Assuming there is only one item in the ETMis datatype which shall
     * always be, but the rest of the class is more generic (the 0 index)
     * @return float
     */
    public float getETx() {
        return etx[0];

    } // getETx() -----------------------------------------------------------

    /**
     * Assuming there is only one item in the ETMis datatype which shall
     * always be, but the rest of the class is more generic (the 0 index)
     * @return float
     */
    public float getETy() {
        return ety[0];

    } // getETy() -----------------------------------------------------------

    /**
     * Assuming there is only one item in the ETMis datatype which shall
     * always be, but the rest of the class is more generic (the 0 index)
     * @return float
     */
    public float getPhi() {
        return phi[0];

    } // getPhi() -----------------------------------------------------------

    /**
     * Returns info string for picking.
     * @param index
     * @return info string
     */
    public String getHitInfo(int index) {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
        if (simpleOutput > 0) {
            String output = getNameScreenName() + " index: " + index;
            if (simpleOutput == 1 || simpleOutput == 3) {
                output += "\n ET=" + String.format("%.3f", et[index]) + " GeV\n "
                        + AMath.PHI + " = " + String.format("%.3f", Math.toDegrees(phi[index])) + AMath.DEGREES;
            }
            if (simpleOutput == 2 || simpleOutput == 3) {
                output += "\n Ex=" + String.format("%.3f", et[index] * Math.cos(phi[index])) + " GeV "
                        + "\n Ey=" + String.format("%.3f", et[index] * Math.sin(phi[index])) + " GeV ";
            }
            return output;
        }

        String msg;

        msg = getNameScreenName() + "\n";
        msg += " storegate key: ";
        msg += (storeGateKey == null ? "n/a" : storeGateKey) + "\n";
        msg += " Sum-ET  = " + String.format("%.3f", sumet[index]) + " GeV" + "\n"
                + " ET-Mis  = " + String.format("%.3f", et[index]) + " GeV" + "\n"
                + " ETx-Mis = " + String.format("%.3f", etx[index]) + " GeV" + "\n"
                + " ETy-Mis = " + String.format("%.3f", ety[index]) + " GeV" + "\n"
                + " " + AMath.PHI + " = "
                + String.format("%.3f", Math.toDegrees(phi[index]))
                + AMath.DEGREES + "  (" + String.format("%.3f", phi[index]) + " rad)";

        return msg;

    } // getHitInfo() -------------------------------------------------------

    /**
     * Info on ETMis contained in v-plot rubberband selection
     * @return String
     */
    public String getVPHitInfo() {
        makeDrawList();
        if (numDraw == 0) {
            return "";
        }

        double sumEt = 0.0;
        for (int i = 0; i < numDraw; i++) {
            sumEt += Math.abs(et[i]);
        }

        String key = getStoreGateKey();
        String msg = numDraw + " " + (key != null ? "" + key : getNameScreenName());
        msg += "  ET = " + String.format("%.1f", sumEt);
        return msg;

    } // getVPHitInfo() -----------------------------------------------------

    /**
     * getParameterGroup()
     * @return parameter group
     */
    public String getParameterGroup() {
        return "ETMis";

    } // getParameterGroup() ------------------------------------------------

    public String getName() {
        return "ETMis";

    } // getName() ----------------------------------------------------------

    public String getNameScreenName() {
        return "ETMis";

    } // getNameScreenName() ------------------------------------------------

    /**
     * internalColor()
     * Implement this atlantis.event.AData method
     * Coloring of the graphical object on the canvas
     * @return number of color types
     */
    protected int internalColor() {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if (colorFunction == 0) {
            colorByConstant();
        } else if (colorFunction == 1) {
            colorByIndex();
        } else if (colorFunction == 2) {
            colorByCollection();
        }

        return 3;

    } // internalColor() ----------------------------------------------------


    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
    	//option to scale line width by energy
        Stroke origStroke = ag.getGraphics2D().getStroke();
        if(parameterStore.getUnknown("ETMis", "LineWidth").getI()==0){
            float factor=0.f;
            if(et[0]<5){
                factor=1.0f;
            }else if(et[0]<10.){
                factor=1.5f;
            }else if(et[0]<15.){
                factor=2.0f;
            }else if(et[0]<20.){
                factor=2.5f;
            }else if(et[0]<25.){
                factor=3.0f;
            }else if(et[0]<30.){
                factor=3.5f;
            }else if(et[0]<35.){
                factor=4.0f;
            }else if(et[0]<40.){
                factor=4.5f;
            }else{
                factor=5.0f;
            }
            ag.getGraphics2D().setStroke(new BasicStroke(factor));
        }
    	
        if(projection instanceof AProjectionYX)
        {
        	// check config whether to draw as arrow or dashed line..
        	if(parameterStore.get("ETMis", "Drawas").getI() == 0) // Arrow = 0, Dashed = 1
        	{
        		drawArrow(window, ag, projection);
        	}
        	else
        	{
        		drawDashed(window, ag, projection);
        	}
        }
        else if(projection instanceof AProjectionVP)
        {
        	// Draw ETMis in VPlot...
        	ACoord centers = window.calculateDisplay(getUser(projection));
            int[] drawlist = centers.index;
            double eLimit = 0.05;
            final int numPoints = 25;
            // hv will contain all the coordinates
            // [2] - x and y value
            // [drawlist.length] - should be "3" for ET Miss object by default
            //                     after the PhiWrap
            // [numPoints] - number of points required for drawing Et missing
            //               object in V-Plot
            double[][][] hv = new double[2][drawlist.length][numPoints];

            for(int i = 0; i < drawlist.length; ++i)
            {
                int list = drawlist[i];

                int d = (int) (Math.sqrt((this.et[list] / eLimit) / Math.PI));
                if(d == 0)
                {
                    d = 1;
                }

                for(int j = 0; j < numPoints; j++)
                {
                    hv[0][i][j] = centers.hv[0][list][i] +
                                  d * Math.cos(Math.PI * 2 * j / (numPoints - 1));
                    hv[1][i][j] = centers.hv[1][list][i] +
                                  d * Math.sin(Math.PI * 2 * j / (numPoints - 1));
                }
            }
            ag.draw(new ACoord(hv, drawlist, this, ACoord.DOTTED_POLYGONS));
        }
        else
        {
            super.draw(window, ag, projection);
        }
        //reset stroke if have changed to scale with energy
        if(parameterStore.getUnknown("ETMis", "LineWidth").getI()==0)
            ag.getGraphics2D().setStroke(origStroke);
    }

    /**
     * drawArrow()
     * Method performs the drawing of the object on the canvas as arrow.
     * @param window
     * @param ag
     * @param projection
     */
    public void drawArrow(AWindow window, AGraphics ag, AProjection2D projection)
    {            
            ACoord centers = window.calculateDisplay(getUser(projection));

            int[] drawlist = centers.index; // draw everything in array

            // scale the arrowhead with zoom...
            double zoomScaling=0;
            Point2D.Double[] corners = window.getUserCorners();
            double widthx=corners[1].x-corners[0].x;
            double widthy=corners[0].y-corners[2].y;
            //average width of window
            double width=Math.sqrt(widthx*widthx+widthy*widthy);
            //calculate scaling (100 is arbitrary constant)
            zoomScaling=100/width;
            double arrowheadScale = 10+10*zoomScaling*Math.log(et[0]+1);
           
            // phiPrime represents the angle of the missingET arrow
            double phiPrime = -1*Math.atan2(ety[0],etx[0]);
            
            // arrowMain contains the start point and end point of the arrow
            double[][][]arrowMain = new double[2][drawlist.length][2];
            
            // scale arrow length with etmiss
            double arrowScale = et[0]*et[0]/250.0; //arbitrary scaling factor
            double arrowCenter = 2.02; // this sets the starting point of the arrow. 2.02 ~ edge of ID
            if (arrowScale > 8) {
            	arrowScale = 8;
            }
            for (int i = 0; i < drawlist.length; i++) {
            	for (int j = 0; j < 2; j++) {
            		arrowMain[0][i][j] = centers.hv[0][i][0] + (arrowCenter + arrowScale * j)*(centers.hv[0][i][1]-centers.hv[0][i][0]);
            		arrowMain[1][i][j] = centers.hv[1][i][0] + (arrowCenter + arrowScale * j)*(centers.hv[1][i][1]-centers.hv[1][i][0]);
            	}
            }
            double[][][]arrowHead1 = new double[2][drawlist.length][2];
            double[][][]arrowHead2 = new double[2][drawlist.length][2];
            for(int i = 0; i < drawlist.length; i++)
            {
            	for(int j=0;j<2;j++)
            	{
            		// set the points for the two parts of the arrow head
            		// 2.591 and 3.691 are the factors to add to the arrow's angle for the correct arrowhead orientation...
            		arrowHead1[0][i][j] = arrowMain[0][0][1] + j*(arrowheadScale*Math.cos(2.591+phiPrime));
            		arrowHead1[1][i][j] = arrowMain[1][0][1] + j*(arrowheadScale*Math.sin(2.591+phiPrime));
            		arrowHead2[0][i][j] = arrowMain[0][0][1] + j*(arrowheadScale*Math.cos(3.691+phiPrime));
            		arrowHead2[1][i][j] = arrowMain[1][0][1] + j*(arrowheadScale*Math.sin(3.691+phiPrime));	
            	}
            }
            // draw arrow, 3 parts, main arrow body and two lines for arrowhead
            ag.draw(new ACoord(arrowMain, drawlist, this, ACoord.DOTTED_POLYGONS));
            ag.draw(new ACoord(arrowHead1, drawlist, this, ACoord.DOTTED_POLYGONS));
            ag.draw(new ACoord(arrowHead2, drawlist, this, ACoord.DOTTED_POLYGONS));
        
    } // drawArrow() -------------------------------------------------------------

    /**
     * drawDashed()
     * Method performs the drawing of the object on the canvas as a dashed line.
     * @param window
     * @param ag
     * @param projection
     */
    public void drawDashed(AWindow window, AGraphics ag, AProjection2D projection)
    {        
            //Stop Etmiss being scaled by Fisheye
            boolean origStatus = parameterStore.get(projection.getName(), "FishEye").getStatus();
            parameterStore.get(projection.getName(), "FishEye").setStatus(false);
            ACoord centers = window.calculateDisplay(getUser(projection));
            if(origStatus) parameterStore.get(projection.getName(), "FishEye").setStatus(origStatus);
            
            int[] drawlist = centers.index; // draw everything in array
            // hv will contain all the the coordinates
            // [2] - x and y value
            // [numData] - should be "1" for ETMis datatype
            // [YX_NUMPOINTS] - number of points required for ET Miss object

            // length of drawlist should always be 1 -> only 1 iteration
            double[][][] hv = new double[2][drawlist.length][YX_NUMPOINTS];
            for(int i = 0; i < drawlist.length; i++)
            {
                for(int j = 0; j < YX_NUMPOINTS; j++)
                {
                    hv[0][i][j] = centers.hv[0][i][j]; // x
                    hv[1][i][j] = centers.hv[1][i][j]; // y
                }
            }
            ag.draw(new ACoord(hv, drawlist, this, ACoord.DOTTED_POLYGONS));
        
    } // drawDashed() -------------------------------------------------------------
   /**
     * getYXUser()
     * calculates coordinates for the Et missing graphical object which
     * consists of one arrowed line, i.e. one line and two smaller lines
     * forming the arrow, all together NUM_POINTS points
     * @return coordinates
     */
    protected ACoord getYXUser()
    {
        if (etx[0] == 0.f && ety[0] == 0.f) return ACoord.NO_DATA;
        
        makeDrawList(); // applycuts will be called here

        // Et missing graphical object consists of one dashed line from center
        // to the border of canvas, or an arrow.

        // hv will contain all the coordinates
        // [2] - x and y value
        // [numData] - should be "1" for ETMis datatype
        // YX_NUMPOINTS - number of points required for ET Miss object
        double[][][] hv = new double[2][numDraw][YX_NUMPOINTS];
        int[] index = new int[numDraw];
        // i counter, here should have just one item in the coordinate array
        // (index.length = 1), numDraw should be 1
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            // main line (dashed)
            // to be properly done: geting border values over canvas/window
            double inc = 10000 / YX_NUMPOINTS;
            double x = Math.cos(phi[list]);
            double y = Math.sin(phi[list]);
            for(int ii = 0; ii < YX_NUMPOINTS; ii++)
            {
                hv[0][i][ii] = x * (inc * ii);
                hv[1][i][ii] = y * (inc * ii);
            }
            
            index[i] = list;
        }
        return new ACoord(hv, index, this);

    } // getYXUser() --------------------------------------------------------

    @Override
    protected ACoord getRZUser() {
        if (etx[0] == 0.f && ety[0] == 0.f) return ACoord.NO_DATA;
        
        double s = 1+Math.sqrt(et[0]);//4*parameterStore.get("ETMis", "Scale").getD();
        if (0 == s || parameterStore.get("ETMis", "Drawas").getI() == 1) {
            return ACoord.NO_DATA;
        }
        makeDrawList(); // applycuts will be called here
        double[][][] hv = new double[2][numDraw][YX_NUMPOINTS + 4];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++) {
            int list = listdl[i];
            double rhoMinus = 500;
            double rhoMax = 1500;
            double inc = 1. / YX_NUMPOINTS;
            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double rhoPlus = rhoMax;
            if (Math.abs(et[list]) < maxEnergy) {
                rhoPlus = (rhoMax - rhoMinus) * s * Math.abs(et[list]) / maxEnergy;
            }

            double updown = 0;
            double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
            if (phiMid > AMath.TWO_PI) {
                phiMid -= AMath.TWO_PI;
            }
            double phiDiff = Math.abs(phi[list] - phiMid);
            if (phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.) {
                updown = 1;
            } else {
                updown = -1;
            }

            //double y = rhoPlus*Math.cos(phiDiff);//to change with cos of phiDiff
            double y = rhoPlus * updown;//to change just by up or down
            for (int ii = 0; ii <= YX_NUMPOINTS; ii++) {
                hv[0][i][ii] = 0;
                hv[1][i][ii] = rhoMinus * updown + y * (inc * ii);
            }
            hv[0][i][YX_NUMPOINTS + 1] = rhoMinus * .1;
            hv[1][i][YX_NUMPOINTS + 1] = rhoMinus * updown + 0.8 * y;
            hv[0][i][YX_NUMPOINTS + 2] = -rhoMinus * .1;
            hv[1][i][YX_NUMPOINTS + 2] = rhoMinus * updown + 0.8 * y;
            hv[0][i][YX_NUMPOINTS + 3] = 0;
            hv[1][i][YX_NUMPOINTS + 3] = rhoMinus * updown + y;

            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES);

    } // getRZUser() --------------------------------------------------------

    /**
     * calculate coordinates for the Et missing graphical object
     * in V-Plot (eta-phi projection)
     * @return coordinates
     */
    protected ACoord getVPUser()
    {
        if (etx[0] == 0.f && ety[0] == 0.f) return ACoord.NO_DATA;
        
        makeDrawList(); // applycuts will be called here

        // numDraw for ET Miss object should be 1
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            h[0] = 0.0; // h[0] is the eta value, ETMis always drawn in the
                        // centre of VPlot - at eta 0
            v[0] = Math.toDegrees(phi[list]); // v[0] is the phi value
            index[i] = list;
        }

        return new ACoord(h, v, index, this).includePhiWrapAround("VP");

    } // getVPUser() --------------------------------------------------------

    /**
     * this method is defined as an abstract method in AData, so must be
     * implemented in this child class
     */
    protected void applyCuts() {

        // no need to cut index (always 0), and transverse momentum for ET Miss
        // assuming only 1 item in the ETMis datatype, thus index 0
        cutPhi(phi[0]);

        // eta value for ET Miss is 0
        cutEta(0.0f);

        // cut on ETMiss (default cut of 15 GeV)
        cut("CutsObjects", "ETMis", " |ETMis|", et);

    } // applyCuts() --------------------------------------------------------

    /**
     * phi cut implementation for ETMis object
     * @param phi float
     */
    protected void cutPhi(float phi) {
        AParameter par = parameterStore.get("CutsATLAS", "CutPhi");
        boolean usePhiCut = par.getStatus();
        if (usePhiCut) {
            double phiCut = Math.toRadians(Math.abs(par.getD()));
            double phiMid = Math.toRadians(
                    AMath.nearestPhiDegrees(parameterStore.get("CutsATLAS", "PhiMiddle").getD()));
            double phiDiff = Math.abs(AMath.nearestPhiRadians(phi, phiMid)
                    - phiMid);

            if (phiDiff - phiCut > 1.0e-6) {
                numDraw = 0;
            }
        }

    } // cutPhi() -----------------------------------------------------------

    /**
     * eta cut implementation for ETMis object
     * @param eta float
     */
    protected void cutEta(float eta) {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");
        boolean useEtaCut = par.getStatus();
        if (useEtaCut) {
            double etaCut = Math.abs(par.getD());
            double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
            double etaDiff = Math.abs(eta - etaMid);
            if (etaDiff - etaCut > 1.0e-6) {
                numDraw = 0;
            }
        }

    } // cutEta() -----------------------------------------------------------
} // class AETMisData =======================================================

