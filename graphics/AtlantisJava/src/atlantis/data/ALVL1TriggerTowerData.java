package atlantis.data;

import java.util.Arrays;

import atlantis.canvas.AWindow;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.ADrawParameters;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.list.AListManager;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;
import atlantis.utils.APolygon;

public class ALVL1TriggerTowerData extends AData
{
    public static final boolean ELECTROMAGNETIC = true;
    public static final boolean HADRONIC = false;

    // here energy is actually Et
    private float[] emEnergy;
    private float[] hadEnergy;
    private float[] sumEnergy;
    private float[] eta;
    private float[] deta_em;
    private float[] deta_had;
    private float[] phi;
    private float[] dphi_em;
    private float[] dphi_had;
    private int[] numADC;
    private int[][] hadADC;
    private int[][] emADC;
    private int[] emBCID;
    private int[] hadBCID;
    private int[] isEMSaturated;
    private int[] isHadSaturated;

    ALVL1TriggerTowerData(AHashMap p, AEvent e)
    {
        super(p,e);

        phi = p.getFloatArray("phi");
        eta = p.getFloatArray("eta");
        emEnergy = p.getFloatArray("emEnergy");
        hadEnergy = p.getFloatArray("hadEnergy");
        sumEnergy = p.getFloatArray("sumEnergy");

        deta_em = new float[numData];
        deta_had = new float[numData];
        dphi_em = new float[numData];
        dphi_had = new float[numData];

        fillGranularity();

        numADC = p.getIntArray("numADC");
        emADC = fillADC(p.getIntArray("emADC"));
        hadADC = fillADC(p.getIntArray("hadADC"));
        emBCID = p.getIntArray("emBCID");
        hadBCID = p.getIntArray("hadBCID");
        isEMSaturated = p.getIntArray("isEMSaturated");
        isHadSaturated = p.getIntArray("isHadSaturated");
    }

    private void fillGranularity()
    {
        for(int i = 0; i < numData; ++i)
        {
            float positiveEta = Math.abs(eta[i]);
            if(positiveEta <= 2.4)
            {
                deta_em[i] = 0.05f;
                dphi_em[i] = (float) (Math.PI / 64.);
                deta_had[i] = 0.05f;
                dphi_had[i] = (float) (Math.PI / 64.);
            }
            else if(positiveEta <= 2.5)
            {
                deta_em[i] = 0.05f;
                dphi_em[i] = (float) (Math.PI / 64.);
                deta_had[i] = 0.1f;
                dphi_had[i] = (float) (Math.PI / 32.);
            }
            else if(positiveEta <= 3.0)
            {
                deta_em[i] = 0.1f;
                dphi_em[i] = (float) (Math.PI / 32.);
                deta_had[i] = 0.1f;
                dphi_had[i] = (float) (Math.PI / 32.);
            }
            else if(positiveEta <= 3.1)
            {
                deta_em[i] = 0.1f;
                dphi_em[i] = (float) (Math.PI / 32.);
                deta_had[i] = 0.05f;
                dphi_had[i] = (float) (Math.PI / 32.);
            }
            else if(positiveEta <= 3.2)
            {
                deta_em[i] = 0.05f;
                dphi_em[i] = (float) (Math.PI / 32.);
                deta_had[i] = 0.2f;
                dphi_had[i] = (float) (Math.PI / 16.);
            }
            else if(positiveEta <= 4.3)
            {
                deta_em[i] = 0.2f;
                dphi_em[i] = (float) (Math.PI / 16.);
                deta_had[i] = 0.2f;
                dphi_had[i] = (float) (Math.PI / 16.);
            }
            else if(positiveEta <= 4.4)
            {
                deta_em[i] = 0.2f;
                dphi_em[i] = (float) (Math.PI / 16.);
                deta_had[i] = 0.35f;
                dphi_had[i] = (float) (Math.PI / 16.);
            }
            else
            {
                deta_em[i] = 0.3f;
                dphi_em[i] = (float) (Math.PI / 16.);
                deta_had[i] = 0.35f;
                dphi_had[i] = (float) (Math.PI / 16.);
            }
        }
    }
    
    public int[][] fillADC(int[] tempADC)
    {
        int[][] ADC=null;
        if(tempADC!=null)
        {
            ADC= new int[numADC.length][0];
            int num=0;
            for(int i=0; i<numADC.length; i++)
            {
                // multiple numbers are associated with each cell
                ADC[i]=new int[numADC[i]];
                for(int j=0; j<numADC[i]; j++)
                {
                    ADC[i][j]=tempADC[num]; // fill in array for each cell
                    num++;
                }
            }
        }  // if(ADC != null)    
        return ADC;
    }

    public void plotPulseShapes(int index)
    {
        if(emADC!=null && hadADC!=null)
        {
            //int[][] data={hadADC[index],emADC[index]};
            String[] labels={"EM trigger ADC counts","HAD trigger ADC counts","Sum trigger ADC counts"};
            int[] sumADC= new int[emADC[index].length];
            String[] axis= new String[3];
            String[] colors = new String[3];
            //EM plot
            if(isEMSaturated[index]==0)
                colors[0]="blue";
            else
                colors[0]="red";
            axis[0]="index (BCID="+emBCID[index]+")";
            //HAD plot
            if(isHadSaturated[index]==0)
                colors[1]="blue";
            else
                colors[0]="red";
            axis[1]="index (BCID="+hadBCID[index]+")";
            //Sum plot
            colors[2]="blue";
            axis[2]="index";
            for(int i=0; i<emADC[index].length; i++)
            {
                sumADC[i]=emADC[index][i]+hadADC[index][i];
            }
            int[][] data={emADC[index],hadADC[index],sumADC};
            String title="LVL1TriggerTower " + id[index] + " ADC counts";
            APulseShapePlot.plotADCCounts(data, title, axis, labels, colors);
        }
        else
        {
            if(emADC!=null)
            {
                int[][] data={emADC[index]};
                String[] axis={"index"};
                APulseShapePlot.plotADCCounts(data, "EM trigger ADC counts", axis);
            }
            if(hadADC!=null)
            {
                int[][] data={hadADC[index]};
                String[] axis={"index"};
                APulseShapePlot.plotADCCounts(data, "HAD trigger ADC counts", axis);
            }
        }
    }
    
    public double getEta(int index)
    {
        return eta[index];
    }

    public double getdEta(int index, boolean emOrHad)
    {
        if(emOrHad == ELECTROMAGNETIC)
            return deta_em[index];
        else
            return deta_had[index];
    }

    public double getPhi(int index)
    {
        return phi[index];
    }

    public double getdPhi(int index, boolean emOrHad)
    {
        if(emOrHad == ELECTROMAGNETIC)
            return dphi_em[index];
        else
            return dphi_had[index];
    }

    public double getET(int index, boolean emOrHad)
    {
        if(emOrHad == ELECTROMAGNETIC)
            return emEnergy[index];
        else
            return hadEnergy[index];
    }

    public double getSumET(int index)
    {
        return sumEnergy[index];
    }

    public String getNameScreenName()
    {
        return "LVL1TriggerTower";
    }

    public String getParameterGroup()
    {
        return "LVL1TriggerTower";
    }

    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+
        "\n E="+String.format("%.3f",sumEnergy[index])+" GeV\n "+
        AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        
        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        msg.append("\n EM Energy = ");
        msg.append(String.format("%.3f",emEnergy[index]));
        msg.append(" GeV\n Had Energy = ");
        msg.append(String.format("%.3f",hadEnergy[index]));
        msg.append(" GeV\n Sum Energy = ");
        msg.append(String.format("%.3f",sumEnergy[index]));
        msg.append(" GeV\n ");
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.3f",eta[index]));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.3f",Math.toDegrees(phi[index])));
        msg.append(AMath.DEGREES);
        msg.append("  (" + String.format("%.3f",phi[index]) + " rad)");

        return msg.toString();
    }

    protected void applyCuts()
    {
        cutIndex();
        cut("CutsCalo", "LVL1TriggerET", " ET", sumEnergy);
    }

    public void applyEMCuts()
    {
        cutPhi(phi, dphi_em);
        cutEtaDEta(eta, deta_em);
    }

    public void applyHadCuts()
    {
        cutPhi(phi, dphi_had);
        cutEtaDEta(eta, deta_had);
    }

    public byte[] getColor(boolean emOrHad)
    {
        internalColor(emOrHad);
        coloring();
        byte[] temp = new byte[id.length];
        for(int i = 0; i < temp.length; i++)
            temp[i] = color[getIndexFromId(id[i])];
        return temp;
    }

    /*
     * Returns the colours of the trigger tower cells
     * If this method is used then internalColor(boolean emOrHad)
     * must be called first elsewhere
     */
    public int[] getColor(int[] dl)
    {
        coloring();
        int[] temp = new int[dl.length];

        for(int i = 0; i < temp.length; i++)
            temp[i] = color[dl[i]];
        return temp;
    }

    /*
     * Colour the cells if in lists or picked
     * If this method is used then internalColor(boolean emOrHad)
     * must be called first elsewhere
     */
    public void coloring()
    {
        int[][] temp = AListManager.getInstance().getColorMapping(this);
        int[] index = temp[0];
        int[] c = temp[1];
        // now add in colours specified in lists

        for(int i = 0; i < index.length; ++i)
            if(c[i] >= 0)
                color[index[i]] = (byte) c[i];
        int others = AListManager.getInstance().getColorOfOthers();
        // need to check if this data could have been picked
        // so that colouring of hits by STr works even if STr
        // is not in list because it wasn't on....
        if(others >= 0 && parameterStore.get("Data", getName()).getStatus())
        {
            boolean[] inList = new boolean[numData];
            for(int i = 0; i < index.length; ++i)
                inList[index[i]] = true;

            for(int i = 0; i < numData; ++i)
                if(!inList[i])
                    color[i] = (byte) others;
        }
    }

    protected int internalColor()
    {
        colorByConstant();
        return 1;
    }

    protected int internalColor(boolean emOrHad)
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();
        if(colorFunction == 0)
            colorByConstant();
        else if(colorFunction ==1)
        {
            if(emOrHad == ELECTROMAGNETIC)
                Arrays.fill(color, (byte) (parameterStore.get("Det", "ECALFill").getI() + 12));
            else
                Arrays.fill(color, (byte) (parameterStore.get("Det", "HCALFill").getI() + 12));
        }
        return 1;
    }

    protected int getDrawOrFill()
    {
        return AGraphics.FILL;
    }

    private void drawVP(AWindow window, AGraphics ag, boolean emOrHad)
    {
        ACoord centers = getVPUser(emOrHad);
        
        if(centers == null)
            return;

        int[] drawlist = centers.index;
        double[] density = new double[drawlist.length];
        double[] sumDensity = new double[drawlist.length];
        double maxDensity = 0.0;
        double[][][] hv = new double[2][drawlist.length][4];  // cell
        int[] index = new int[drawlist.length];
        ACoord geoCell;

        for(int i=0; i<drawlist.length; ++i)
        {
            int list = drawlist[i];
            // y coordinate of four cell corners
            hv[1][i][0] = centers.hv[1][0][i] + Math.toDegrees(getdPhi(list, emOrHad));
            hv[1][i][1] = centers.hv[1][0][i] - Math.toDegrees(getdPhi(list, emOrHad));
            hv[1][i][2] = centers.hv[1][0][i] - Math.toDegrees(getdPhi(list, emOrHad));
            hv[1][i][3] = centers.hv[1][0][i] + Math.toDegrees(getdPhi(list, emOrHad));
            // x coordinate of four cell corners
            hv[0][i][0] = centers.hv[0][0][i] - getdEta(list, emOrHad);
            hv[0][i][1] = centers.hv[0][0][i] - getdEta(list, emOrHad);
            hv[0][i][2] = centers.hv[0][0][i] + getdEta(list, emOrHad);
            hv[0][i][3] = centers.hv[0][0][i] + getdEta(list, emOrHad);

            index[i] = list;

            // calculate density for all cells, and find min and max density
            density[i] = Math.abs(getET(list, emOrHad)) / (getdPhi(list, emOrHad) * getdEta(list, emOrHad));
            sumDensity[i] = Math.abs(getSumET(list)) / (getdPhi(list, emOrHad) * getdEta(list, emOrHad));
            if(i == 0)
                maxDensity = sumDensity[i];
            else
            {
                if (sumDensity[i] > maxDensity)
                    maxDensity = sumDensity[i];
            }
        }

        geoCell = window.calculateDisplay(new ACoord(hv, index, this, ACoord.POLYGONS));
        drawGeometry(geoCell, window, ag, density);
        for (int i = 0; i < density.length; i++)
        {
            double factor = Math.sqrt(density[i] / maxDensity);
            APolygon.scale(geoCell.hv[0][i], geoCell.hv[1][i], factor);
        }

        drawHits(geoCell, window, ag, density);
    }

    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
        int energyType = parameterStore.get(PARAMETER_GROUP, "EnergyType").getI();
        if(projection instanceof AProjectionYX)
        {
            if(energyType == 0 || energyType == 2)
            {
                internalColor(ELECTROMAGNETIC);
                ag.draw(window.calculateDisplay(projection.nonLinearTransform(getYXUser(ELECTROMAGNETIC))));
            }
            if(energyType == 1 || energyType == 2)
            {
                internalColor(HADRONIC);
                ag.draw(window.calculateDisplay(projection.nonLinearTransform(getYXUser(HADRONIC))));
            }
        }
        else if(projection instanceof AProjectionFR)
        {
            if(energyType == 0 || energyType == 2)
            {
                internalColor(ELECTROMAGNETIC);
                ag.draw(window.calculateDisplay(projection.nonLinearTransform(getFRUser(ELECTROMAGNETIC))));
            }
            if(energyType == 1 || energyType == 2)
            {
                internalColor(HADRONIC);
                ag.draw(window.calculateDisplay(projection.nonLinearTransform(getFRUser(HADRONIC))));
            }
        }
        else if(projection instanceof AProjectionRZ)
        {
            if(energyType == 0 || energyType == 2)
            {
                internalColor(ELECTROMAGNETIC);
                ag.draw(window.calculateDisplay(projection.nonLinearTransform(getRZUser(ELECTROMAGNETIC))));
            }
            if(energyType == 1 || energyType == 2)
            {
                internalColor(HADRONIC);
                ag.draw(window.calculateDisplay(projection.nonLinearTransform(getRZUser(HADRONIC))));
            }
        }
        else if(projection instanceof AProjectionVP)
        {
            if(energyType == 0 || energyType == 2)
            {
                internalColor(ELECTROMAGNETIC);
                drawVP(window, ag, ELECTROMAGNETIC);
            }
            if(energyType == 1 || energyType == 2)
            {
                internalColor(HADRONIC);
                drawVP(window, ag, HADRONIC);
            }
        }
        else
        {
            super.draw(window, ag, projection);
        }
    }

    private ACoord getYXUser(boolean emOrHad)
    {
        makeDrawList();
        if(emOrHad == ELECTROMAGNETIC)
            applyEMCuts();
        else
            applyHadCuts();

        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        for(int i=0; i<numDraw; i++)
        {
            int list = listdl[i];
            double et = getET(list, emOrHad);
            if(et == 0.0)
                continue;
            double dphi = getdPhi(list, emOrHad);
            double phiPlus = phi[list] + dphi;
            double phiMinus = phi[list] - dphi;
            double cosPlus = Math.cos(phiPlus);
            double sinPlus = Math.sin(phiPlus);
            double cosMinus = Math.cos(phiMinus);
            double sinMinus = Math.sin(phiMinus);

            double rhoMax;
            double rhoMinus;

            if(emOrHad == ELECTROMAGNETIC)
            {
                // Rho range of LAr Detector (EM) is about between [148.175, 198.47]
                rhoMax = 195;
                rhoMinus = 155;
            }
            else
            {
                // Rho range of TILE Detector (Had) is about between [229, 386]
                rhoMax = 380;
                rhoMinus = 235;
            }
            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double rhoPlus = rhoMax;
            if(Math.abs(et) < maxEnergy)
                rhoPlus = rhoMinus + (rhoMax - rhoMinus) * Math.abs(et) / maxEnergy;

            // 4 corners of the cell area
            // x0, y0
            hv[0][i][0] = rhoMinus * cosPlus;
            hv[1][i][0] = rhoMinus * sinPlus;
            // x1, y1
            hv[0][i][1] = rhoPlus * cosPlus;
            hv[1][i][1] = rhoPlus * sinPlus;
            // x2, y2
            hv[0][i][2] = rhoPlus * cosMinus;
            hv[1][i][2] = rhoPlus * sinMinus;
            // x3, y3
            hv[0][i][3] = rhoMinus * cosMinus;
            hv[1][i][3] = rhoMinus * sinMinus;

            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.POLYGONS);
    }

    protected ACoord getFRUser(boolean emOrHad)
    {
        ACoord coordFR = getYXUser(emOrHad).convertYXToFR().includePhiWrapAround("FR");
        return coordFR;
    }

    protected ACoord getRZUser(boolean emOrHad)
    {
        makeDrawList();
        if(emOrHad == ELECTROMAGNETIC)
            applyEMCuts();
        else
            applyHadCuts();

        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        for(int i=0; i<numDraw; i++)
        {
            int list = listdl[i];
            double et = getET(list, emOrHad);
            if(et == 0.0)
                continue;

            double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
            double phiDiff = Math.abs(phi[list]-phiMid);
            double rSign;
            if (phiDiff > Math.PI/2. && phiDiff <= 3*Math.PI/2.)
                rSign = -1;
            else
                rSign = 1;

            // calculate theta based on the eta value
            double theta = Math.atan(Math.exp(-Math.abs(eta[list]))) * 2.0;
            double deta = getdEta(list, emOrHad);
            double thetaPlus = Math.atan(Math.exp(-(Math.abs(eta[list]) - deta))) * 2.0;
            double thetaMinus = Math.atan(Math.exp(-(Math.abs(eta[list]) + deta))) * 2.0;

            if ((eta[list] > 0.) && (rSign == -1))
            {
                theta = 2 * Math.PI - theta;
                thetaPlus = 2 * Math.PI - thetaPlus;
                thetaMinus = 2 * Math.PI - thetaMinus;
            }
            else if ((eta[list] < 0.) && (rSign == -1))
            {
                theta += Math.PI;
                thetaPlus += Math.PI;
                thetaMinus += Math.PI;
            }
            else if ((eta[list] < 0.) && (rSign == 1))
            {
                theta = Math.PI - theta;
                thetaPlus = Math.PI - thetaPlus;
                thetaMinus = Math.PI - thetaMinus;
            }

            double cosPlus = Math.cos(thetaPlus);
            double sinPlus = Math.sin(thetaPlus);
            double cosMinus = Math.cos(thetaMinus);
            double sinMinus = Math.sin(thetaMinus);

            // decide the region based on the theta value
            final byte LAR = 0;
            final byte LAR_ENDCAP = 1;
            final byte FCAL_EM = 2;
            byte region = LAR;
            // hard-coded value is based on the values in AGeometry.xml
            if(Math.abs(Math.tan(theta)) >= 0.0778 && Math.abs(Math.tan(theta)) < 0.4828)
                region = LAR_ENDCAP;
            else if(Math.abs(Math.tan(theta)) < 0.0778)
                region = FCAL_EM;

            double radiusMinus = 0.;
            double radiusMax = 0.;
            switch(region)
            {
                // use fixed rho/z to determine the lower radius value
                case LAR:
                    if(emOrHad == ELECTROMAGNETIC)
                    {
                        radiusMax = 195 / Math.abs(Math.sin(theta));
                        radiusMinus = 155 / Math.abs(Math.sin(theta));
                    }
                    else
                    {
                        radiusMax = 380 / Math.abs(Math.sin(theta));
                        radiusMinus = 235 / Math.abs(Math.sin(theta));
                    }
                    break;
                case LAR_ENDCAP:
                    if(emOrHad == ELECTROMAGNETIC)
                    {
                        radiusMax = 420 / Math.abs(Math.cos(theta));
                        radiusMinus = 380 / Math.abs(Math.cos(theta));
                    }
                    else
                    {
                        radiusMax = 585 / Math.abs(Math.cos(theta));
                        radiusMinus = 440 / Math.abs(Math.cos(theta));
                    }
                    break;
                case FCAL_EM:
                    if(emOrHad == ELECTROMAGNETIC)
                    {
                        radiusMax = 510 / Math.abs(Math.cos(theta));
                        radiusMinus = 470 / Math.abs(Math.cos(theta));
                    }
                    else
                    {
                        radiusMax = 600 / Math.abs(Math.cos(theta));
                        radiusMinus = 520 / Math.abs(Math.cos(theta));
                    }
                    break;
            }

            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double radiusPlus = radiusMax;
            if(Math.abs(et) < maxEnergy)
                radiusPlus = radiusMinus + (radiusMax - radiusMinus) * Math.abs(et) / maxEnergy;

            // 4 corners of the cell area
            // x0, y0
            hv[0][i][0] = radiusMinus * cosPlus;
            hv[1][i][0] = radiusMinus * sinPlus;
            // x1, y1
            hv[0][i][1] = radiusPlus * cosPlus;
            hv[1][i][1] = radiusPlus * sinPlus;
            // x2, y2
            hv[0][i][2] = radiusPlus * cosMinus;
            hv[1][i][2] = radiusPlus * sinMinus;
            // x3, y3
            hv[0][i][3] = radiusMinus * cosMinus;
            hv[1][i][3] = radiusMinus * sinMinus;

            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.POLYGONS);
    }

    protected ACoord getVPUser(boolean emOrHad)
    {
        makeDrawList();
        if(emOrHad == ELECTROMAGNETIC)
            applyEMCuts();
        else
            applyHadCuts();

        if(numDraw == 0)
            return null;
        
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index=new int[numDraw];

        for(int i=0; i<numDraw; i++)
        {
            int list=listdl[i];
            h[i]=eta[list];
            v[i]=Math.toDegrees(phi[list]);
            index[i]=list;
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("VP");
    }

    private void drawGeometry(ACoord display, AWindow window, AGraphics ag, double[] density)
    {
        boolean drawCellGeometry = parameterStore.get(getParameterGroup(), "CellGeometry").getStatus();
        boolean drawCellOutline = parameterStore.get(getParameterGroup(), "CellOutline").getStatus();
        int cellGeometryColor = parameterStore.get(getParameterGroup(), "CellGeometry").getI();
        int cellOutlineColor = parameterStore.get(getParameterGroup(), "CellOutline").getI();
        boolean drawFrame = parameterStore.get(getParameterGroup(), "Frame").getStatus();
        int frameColor = parameterStore.get(getParameterGroup(), "Frame").getI();
        int frameWidth = parameterStore.get(getParameterGroup(), "FrameWidth").getI();
        //only draw frames for Grey/BW color maps if is selected to draw frames
        if(drawFrame && AColorMap.drawFrames())
            drawFrame = true;
        else
            drawFrame = false;
        
        if(!drawCellGeometry && !drawCellOutline)
            return;

        // Draw geometry
        for(int i=0; i<display.hv[0].length; ++i)
        {
            if(density[i] == 0.0)
                continue;

            if(drawCellGeometry)
            {
                if(drawFrame)
                {
                    ag.updateDrawParameters(new ADrawParameters(true, frameColor, 1, 0, frameWidth, 0));
                    ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
                }
                ag.updateDrawParameters(new ADrawParameters(true, cellGeometryColor, 1, 0, 0, 0));
                ag.fillPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }
            if(drawCellOutline)
            {
                ag.updateDrawParameters(new ADrawParameters(true, cellOutlineColor, 0, 1, 0, 0, false, 1, 0));
                ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }
        }
    }

    private void drawHits(ACoord display, AWindow window, AGraphics ag, double[] density)
    {
        int[] color = this.getColor(display.index);

        boolean drawCellGeometry = parameterStore.get(getParameterGroup(), "CellGeometry").getStatus();
        boolean drawFrame = parameterStore.get(getParameterGroup(), "Frame").getStatus();
        int frameColor = parameterStore.get(getParameterGroup(), "Frame").getI();
        int frameWidth = parameterStore.get(getParameterGroup(), "FrameWidth").getI();
        int lineWidth = parameterStore.get(getParameterGroup(), "LineWidth").getI();

        for(int i=0; i<display.hv[0].length; ++i)
        {
            if(density[i] == 0.0)
                continue;

            // draw frame
            if (drawFrame && !drawCellGeometry)
            {
                ag.updateDrawParameters(new ADrawParameters(true, frameColor, 1, 0, frameWidth, 0));
                ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }

            // draw hits
            ag.updateDrawParameters(new ADrawParameters(true, color[i], 1, lineWidth, 0, 0));
            ag.setCurrentDataAndIndex(this, display.index[i]);
            ag.fillPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            ag.clearCurrentDataAndIndex();
        }
    }
}
