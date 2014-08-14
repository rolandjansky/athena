package atlantis.data;

import java.util.ArrayList;
import java.util.List;

import atlantis.event.AAssociation;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.event.AObjectsAssociation;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.parameters.APar;
import atlantis.utils.AMath;
import atlantis.utils.A3Vector;
import atlantis.utils.A4Vector;
import atlantis.utils.AHashMap;
import atlantis.canvas.AWindow;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionVP;
import atlantis.graphics.AGraphics;

/**
 * Jet class - class handling multiple Jet datatypes
 *    (multiple collections) from various Jet alrogithms
 *    (JetRecJet, ParticleJet, etc)
 */
public class AJetData extends AData
{
    protected float[] eT;
    protected float[] eta;
    protected float[] phi;
    protected float[] mass;
    protected float[] pT;
    protected float[] px;
    protected float[] py;
    protected float[] pz;
    //Transition variable to ensure backwards compatability
    protected boolean has4Vect = false;
    //variables for jet quality information
    protected float[] quality = null;
    protected float[] qualityLAr = null;
    protected float[] qualityTile = null;
    protected float[] time = null;
    protected float[] timeClusters = null;
    protected int[] n90cells = null;
    protected int[] n90const = null;
    protected float[] hecf = null;
    protected float[] emfrac = null;
    protected float[] tileGap3f = null;
    protected float[] fcorCell = null;
    protected float[] fcorDotx = null;
    protected float[] fcorJet = null;
    protected float[] fcorJetForCell = null;
    protected int[] nbadcells = null;
    protected float[] fracSamplingMax = null;
    protected float[] sMax = null;
    protected float[] OutOfTimeEfrac = null;
    protected float[] isGood = null;
    protected float[] isBad = null;
    protected float[] isUgly = null;
    protected String[] bTagName = null;              //new bTagname variable
    protected float[]  bTagValue = null;             //new bTagValue variable
    protected float[] weight = null;
    protected float[] jvf = null;
    //Transition variable to ensure backwards compatability
    protected boolean hasJvf = true;

    String[] caloSample = { "EMB0", "EMB1", "EMB2", "EMB3", "EME0", "EME1", "EME2", "EME3", "HEC0", "HEC1", "HEC2", "HEC3", "TileBar0", "TileBar1", "TileBar2", "TileGap1", "TileGap2", "TileGap3", "TileExt0", "TileExt1", "TileExt2", "FCAL0", "FCAL1", "FCAL2", "Unknown" };


    AJetData(AHashMap p, AEvent e)
    {
        super(p,e);
        if (p.get("et") != null)
        {
            eT = p.getFloatArray("et");
        }
        else
        {
            eT = p.getFloatArray("pt");
        }


        if(p.get("mass") != null)
        {
            mass = p.getFloatArray("mass");
            px = p.getFloatArray("px");
            py = p.getFloatArray("py");
            pz = p.getFloatArray("pz");
            has4Vect = true;
        }
        eta = p.getFloatArray("eta");
        phi = p.getFloatArray("phi");

        if(p.get("emfrac") != null)
        {
            emfrac = p.getFloatArray("emfrac");
        }

        float forwardJVF = 10.0f;
        float initJVF = 1.0f;
        if(p.get("jvf") != null)
        {
            jvf = p.getFloatArray("jvf");
            for(int i=0; i<(jvf.length); i++)
            {
                if( jvf[i] == -1.0 )
                    jvf[i] = forwardJVF; // forward-jets have jvf=-1
            }
        }
        else // dummy values for older xml-files
        {
            hasJvf = false;
            jvf = new float[eta.length]; // for array size
            for(int i=0; i<(jvf.length); i++)
            {
                jvf[i] = initJVF; // need to assign dummy value to allow cut
            }
        }

        //Getting BTagNames and BTagValues from xml       
        if(p.get("bTagValue") != null)
        {
            bTagValue = p.getFloatArray("bTagValue");
            bTagName  = p.getUnsureStringArray("bTagName");
            weight    = new float[eta.length];                      
            for(int i=0; i<(eta.length); i++)
                weight[i] = bTagValue[i*(bTagValue.length/eta.length) + 0]; 
            // 0 is the first instance of JetFitterNNComb
            // we take this as default for now...
        }

        if(has4Vect)
        {
            float[] pTtemp = new float[mass.length];
            for(int i = 0; i < mass.length; i++){
                pTtemp[i] = (float) (new A4Vector(px[i],py[i],pz[i],mass[i]) ).getPt();
            }
            pT = pTtemp;
        }

        if(p.get("quality") != null)
            quality         = p.getFloatArray("quality");
        if(p.get("qualityLAr") != null){
            qualityLAr      = p.getFloatArray("qualityLAr");
            qualityTile     = p.getFloatArray("qualityTile");
            time            = p.getFloatArray("time");
            timeClusters    = p.getFloatArray("timeClusters");
            n90cells        = p.getIntArray("n90cells");
            n90const        = p.getIntArray("n90const");
            hecf            = p.getFloatArray("hecf");
            tileGap3f       = p.getFloatArray("tileGap3f");
            fcorCell        = p.getFloatArray("fcorCell");
            fcorDotx        = p.getFloatArray("fcorDotx");
            fcorJet         = p.getFloatArray("fcorJet");
            fcorJetForCell  = p.getFloatArray("fcorJetForCell");
            nbadcells       = p.getIntArray("nbadcells");
            fracSamplingMax = p.getFloatArray("fracSamplingMax");
            sMax            = p.getFloatArray("sMax");
            OutOfTimeEfrac  = p.getFloatArray("OutOfTimeEfrac");
            isGood          = p.getFloatArray("isGood");
            isBad           = p.getFloatArray("isBad");
            isUgly          = p.getFloatArray("isUgly");
        }

        String assocKey = getFullName();
        // create Jet - Calo associations if cells information is available
        if(p.get("numCells") != null)
        {
            event.getAssociationManager().add(new AAssociation(assocKey, "LAr", p.getIntArray("numCells"), p.getIntArray("cells"),event));
            event.getAssociationManager().add(new AAssociation(assocKey, "TILE", p.getIntArray("numCells"), p.getIntArray("cells"),event));
            event.getAssociationManager().add(new AAssociation(assocKey, "HEC", p.getIntArray("numCells"), p.getIntArray("cells"),event));
            event.getAssociationManager().add(new AAssociation(assocKey, "FCAL", p.getIntArray("numCells"), p.getIntArray("cells"),event));
        }

        if (p.get("clusterKey") != null)
        {
            int[] clusterLinkCount = p.getUnsureIntArray("clusterLinkCount");
            event.getAssociationManager().add(new AObjectsAssociation(assocKey, "Cluster", 
                    p.getStringArray("clusterKey"), p.getIntArray("clusterIndex"), clusterLinkCount, event));
        }
        if (p.get("trackKey") != null)
        {
            int[] trackLinkCount = p.getUnsureIntArray("trackLinkCount");
            event.getAssociationManager().add(new AObjectsAssociation(assocKey, "Track", 
                    p.getStringArray("trackKey"), p.getIntArray("trackIndex"), trackLinkCount, event));
        }        
    }


    public String getParameterGroup()
    {
        return "Jet";
    }


    public String getName()
    {
        return "Jet";
    }


    public String getNameScreenName()
    {
        return "Jet";
    }


    public float getET(int index)
    {
        return eT[index];
    }


    public float getEta(int index)
    {
        return eta[index];
    }


    public float getPhi(int index)
    {
        return phi[index];
    }


    public float[] getET()
    {
        return eT;
    }


    public float[] getEta()
    {
        return eta;
    }


    public float[] getPhi()
    {
        return phi;
    }

    public float[] getPt()
    {
        if(has4Vect){
            return pT;
        }else{
            return eT;
        }
    }

    @SuppressWarnings("unused")
    // TODO: these cuts are from Nikolina
    // using the wrong indices?
    private float[] getfCorJetLevel()
    {
        // Tom - problem here with inconsistent indices?
        // this code is not used yet, but the Jet people want it eventually
        // i is the total data index, but we need to get that from the draw list, listdl[i].
        float[] temp = new float[numDraw];
        for (int i = 0; i < numDraw; i++)
            temp[i] = (float) (fcorCell[i] - fcorJet[i]);
        return temp;
    }

    @SuppressWarnings("unused")
    // TODO: these cuts are from Nikolina
    // using the wrong indices?
    private float[] getHECfQ()
    {
        float[] temp = new float[numDraw];
        for (int i = 0; i < numDraw; i++)
            temp[i] = (float) (Math.abs(hecf[i]) + Math.abs(quality[i]));
        return temp;
    }

    /* 
     *  Return an ordered list of the btaggers available in the event.
     *  If using an older XML file without btaggers available, an empty list is returned.
     *  Ordering is important to be consistent with the BTag Weights, which are ordered the same.
     */
    public List<String> getBTaggers() {

        List<String> taggers = new ArrayList<String>();

        if(bTagName == null)
            return taggers;

        for(String tagger : bTagName) {
            if(!taggers.contains(tagger)) {
                taggers.add(tagger);
            }
        }

        return taggers;
    } 

    protected void applyCuts()
    {
        cutIndex();
        cut("CutsObjects", "JetET", " |ET|", eT);
        cutPhi(phi);
        cutEta(eta);
        cut("CutsObjects", "JetEMfraction", " |EM fraction|", emfrac);
        cut("CutsObjects", "JetVxFraction", " |Vx Fraction|", jvf);

        /* Placeholder for jet quality cuts (added by Nikolina Ilic)
        cut("CutsObjects", "JetTime", " |dt(jet,coll)|", time);
        cut("CutsObjects", "JetTileGap3F", " TileGap3F", tileGap3f);
        cut("CutsObjects", "JetfCORRcell", " Already corrected cell level correction", fcorCell);
        cut("CutsObjects", "JetfCORRjetLevel", " Correction by jet level estimation", getfCorJetLevel());
        cut("CutsObjects", "JetHECfQ", " HECf-Quality", getHECfQ());
        cut("CutsObjects", "JetOOTEf", " Out-Of-Time Energy frac", OutOfTimeEfrac);
         */
    }

    public void colorbyBJets() {
        //Initialy color all jets with constant color   		 		 
        for (int i = 0; i < numData; i++) {
            int constantColor2 = parameterStore.get("Jet", "Constant").getI();
            color[i] = (byte) constantColor2;
        }

        //Read necessary information to colour Jets
        int bTagValue_user =  parameterStore.get("CutsObjects", "JetBTagger").getI();   //getting user B-tagger preference 
        int constantColor = parameterStore.get("BJet", "Constant").getI();
        int constantColor2 = parameterStore.get("Jet", "Constant").getI();

        //if b-tagging information is contained in xml file
        if(bTagValue != null) {
            int mod = bTagValue.length / eta.length;		//Number of Btaggers - eta.length is the number of Jets  			  
            double s = parameterStore.get("CutsObjects", "JetBTagweight").getD();
            for (int i = 0; i < eta.length; i++) {
                color[i] = (byte) constantColor2;
                if(bTagValue[bTagValue_user + i*mod]>s){
                    color[i] = (byte) constantColor;
                }
            }
        }
    }




    protected int internalColor()
    {
        int colorFunction = APar.instance().get(PARAMETER_GROUP, "ColorFunction").getI();

        if (colorFunction == 0)
        {
            colorByConstant();
        }
        else if(colorFunction == 1)
        {
            colorByIndex();
        }
        else if(colorFunction == 2)
        {
            colorByCollection();
        }
        else if (colorFunction == 3)
        {
            colorbyBJets();
        }
        return 3;
    }

    public String getHitInfo(int index)
    {
        int simpleOutput = AGlobals.instance().getSimpleOutput();
        if(simpleOutput>0){
            String output = getNameScreenName()+" index: " + index;
            if(simpleOutput==1 || simpleOutput==3) 
                output+= "\n ET="+String.format("%.3f",eT[index])+" GeV\n "+
                        AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
                        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
            if(simpleOutput==2 || simpleOutput==3) 
                output+= "\n Ex="+String.format("%.3f",eT[index]*Math.cos(phi[index]))+" GeV "+
                        "\n Ey="+String.format("%.3f",eT[index]*Math.sin(phi[index]))+" GeV "+
                        "\n Ez="+String.format("%.3f",eT[index]*Math.sinh(eta[index]))+" GeV ";
            return output;
        }

        String msg;
        String k = this.getStoreGateKey();
        String sgKey = k != null ? k : "n/a";
        String printPt = "";
        if(has4Vect){
            printPt = "\n PT = " + String.format("%.3f",pT[index]) + " GeV";
        }
        msg = getNameScreenName() + " (id: " + id[index] + " index: " + index + ")\n" +
                " storegate key: " + sgKey + printPt + "\n ET   = " +
                String.format("%.3f",eT[index]) + " GeV" + "\n E   = " +
                String.format("%.3f",Math.abs(eT[index] / Math.cos(AMath.lambda(eta[index])))) +
                " GeV" + "\n " + AMath.ETA + " = " + String.format("%.3f",eta[index]) + "\n " +
                AMath.PHI + " = " + String.format("%.3f",Math.toDegrees(phi[index])) +
                AMath.DEGREES + "  (" + String.format("%.3f",phi[index]) + " rad)";

        if(isGood != null) msg += "\n Selection: isGood =" + isGood[index] + "(isBad = " + isBad[index] + ", isUgly = " + isUgly[index] + ")";
        if(hecf != null) msg += "\n hecf = " + String.format("%.2f",hecf[index])+ ", n90cells = " + n90cells[index] + " (n90const = " + n90const[index] + ")";
        if (emfrac != null) msg += "\n emfrac = " + String.format("%.2f",emfrac[index]);
        if (quality != null) msg += "\n quality = " + String.format("%.2f",quality[index]);
        if (qualityLAr != null) msg += "(qLAr = " + String.format("%.2f",qualityLAr[index]) + ")";

        if (hasJvf)
        {
            msg += "\n jvf = " + String.format("%.2f",jvf[index]);	
        }
        else
        {
            msg += "\n jvf n/a ";	
        }
        if (weight != null) msg += "  b-weight = " + String.format("%.2f",weight[index]);	
        // Displaying bTagValues and bTagnames to the outputscreen	
        if(bTagValue != null)
        { 	
            //calculating "multiple" number
            int Div = (bTagValue.length)/(eta.length);
            msg += "\n b-Taggers: ";
            for (int s=0;s<Div;s++)
            {
                msg += bTagName[index*Div + s] + "=" + String.format("%.2f",bTagValue[index*Div + s]) + ", "; 
            }    		    	
        }
        if (time != null) msg += "\n time = " + String.format("%.2f",time[index]) + " ns (clus time = " + String.format("%.2f",timeClusters[index]) + " ns)";  
        if (OutOfTimeEfrac != null) msg += "\n Out-Of-Time Energy fraction = " + String.format("%.2f", OutOfTimeEfrac[index]);   
        if (fracSamplingMax != null) msg += "\n fracSamplingMax = " + String.format("%.2f",fracSamplingMax[index]) + " (sMax = " + caloSample[(int) sMax[index]] + ")";
        if (tileGap3f != null) msg += "\n tileGap3f = " + String.format("%.2f",tileGap3f[index]) + ", fcorCell=" + String.format("%.2f",fcorCell[index]) + ", fcorJet=" + String.format("%.2f",fcorJet[index]);
        if (fcorDotx != null) msg += "\n fcorDotx=" + String.format("%.2f",fcorDotx[index]) + " (not implemented yet)";

        return msg;
    }

    // info on Jet collections contained in v-plot rubberband selection
    public String getVPHitInfo()
    {
        makeDrawList();
        if (numDraw == 0)
            return "";
        double sumE = 0.;
        double sumEt = 0.;

        for (int i = 0; i < numDraw; ++i)
        {
            sumEt += Math.abs(eT[listdl[i]]);
            sumE += Math.abs(eT[listdl[i]] / Math.cos(AMath.lambda(eta[listdl[i]])));
        }

        String key = getFullName();
        String msg = numDraw + " " + (key != null ? "" + key : getNameScreenName());
        msg += "  sum(ET) = " + String.format("%.1f",sumEt) + "  sum(E) = " + String.format("%.1f",sumE);

        return msg;
    }

    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        if(has4Vect){
            for (int i =    0; i < num; ++i)
            {
                int k = list[i];
                A4Vector start = new A4Vector();
                start.setPtEtaPhiM(pT[k],eta[k],phi[k],mass[k]);
                sum.add(start);
            }
        }else{
            for (int i =    0; i < num; ++i)
            {
                int k = list[i];
                A3Vector start = A3Vector.fromRhoPhiZ(0., 0., 0.);
                double tL = AMath.tanLambda(eta[k]);
                A3Vector stop = A3Vector.fromRhoPhiZ(1., phi[k], tL);
                A3Vector v = (stop.subtract(start)).normalize();
                double p = eT[k] / Math.sqrt(1. - v.z * v.z);
                sum.add(new A4Vector(v.scale(p), 0.));
            }
        }
        return sum;
    }

    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
        if (projection instanceof AProjectionVP)
        {
            ACoord centers = window.calculateDisplay(getUser(projection));
            int[] drawlist = centers.index;
            double eLimit = 0.05;
            int numPoints = 25;
            double[][][] hv = new double[2][drawlist.length][numPoints];

            for (int i = 0; i < drawlist.length; ++i)
            {
                int list = drawlist[i];
                int jetMode = parameterStore.get("VP", "Jet").getI();
                double e;
                if (jetMode == 0)
                    // showing jet in ET
                    e = Math.abs(eT[list]);
                else
                    // showing jet in E
                    e = Math.abs(eT[list] / Math.cos(AMath.lambda(eta[list])));
                int d = (int) (Math.sqrt((e / eLimit) / Math.PI));
                if (d == 0)
                    d = 1;
                for (int j = 0; j < numPoints; j++)
                {
                    hv[0][i][j] = centers.hv[0][0][i] + d * Math.cos(Math.PI * 2 * j / (numPoints - 1));
                    hv[1][i][j] = centers.hv[1][0][i] + d * Math.sin(Math.PI * 2 * j / (numPoints - 1));
                }
            }
            ag.draw(new ACoord(hv, drawlist, this, ACoord.POLYLINES));
        }
        else
        {
            super.draw(window, ag, projection);
        }
    }

    protected int getDrawOrFill()
    {
        return AGraphics.FILL;
    }

    protected ACoord getVPUser()
    {
        makeDrawList();
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            h[i] = eta[list];
            v[i] = Math.toDegrees(phi[list]);
            index[i] = list;
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("VP");
    }

    @Override
    protected ACoord getYXUser()
    {
        int coneSmooth = 20;
        makeDrawList();
        double[][][] hv = new double[2][numDraw][(2*coneSmooth)];
        int[] index = new int[numDraw];

        String k = this.getStoreGateKey();
        String sgKey = k != null ? k : "n/a";

        double dphi = 0.4;

        if(sgKey.contains("Kt6"))
            dphi = 0.6;
        if(sgKey.contains("Kt1"))
            dphi = 1.0;

        double rhoMinus = parameterStore.get("Jet","Innerdrawradius").getD();
        double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
        double s = parameterStore.get("Jet", "Scale").getD();
        double rhoMax = s*1000;
        if (0==s) return ACoord.NO_POLYLINES;

        for(int i=0; i<numDraw; i++)
        {
            int list = listdl[i];
            double phiPlus = phi[list] + dphi;
            double phiMinus = phi[list] - dphi;

            //if no composite particles increase jet size
            //Vector keys = (Vector) event.getCollections().get("CompositeParticle");
            //if (keys == null || !parameterStore.get("Data", "CompositeParticle").getStatus()) rhoMax=1000;//1800;

			double rhoPlus = rhoMax;
			if(Math.abs(eT[list]) < maxEnergy) {
				if(parameterStore.get("Jet", "ScaleType").getI() == 1) //sq root
					rhoPlus = rhoMinus + rhoMax *  (Math.sqrt  (Math.abs(eT[list])/maxEnergy));
				if(parameterStore.get("Jet", "ScaleType").getI() == 0) // linear
					rhoPlus = rhoMinus + rhoMax *  (Math.abs(eT[list])/maxEnergy);
			}

            double vx = parameterStore.get("Event", "XVtx").getD();
            double vy = parameterStore.get("Event", "YVtx").getD();

            double h_shift = vx;
            double v_shift = vy;

            // the loop interpolates between the inner and outer edge of the jet to draw the arc

            // draw the outer arc
            for(int j=0; j<coneSmooth; j++){
                float f = ((float)j)/(coneSmooth-1);
                hv[0][i][j] = h_shift + (rhoPlus) * Math.cos(((1-f)*phiPlus + (f)*phiMinus));
                hv[1][i][j] = v_shift + (rhoPlus) * Math.sin(((1-f)*phiPlus + (f)*phiMinus));
            }

            // draw the inner arc
            for(int j=coneSmooth; j<(2*coneSmooth); j++){
                float f = ((float)j-coneSmooth)/(coneSmooth-1);
                hv[0][i][j] = h_shift + (rhoMinus) * Math.cos(((1-f)*phiMinus + (f)*phiPlus));
                hv[1][i][j] = v_shift + (rhoMinus) * Math.sin(((1-f)*phiMinus + (f)*phiPlus));
            }

            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.TRANSPARENT_POLYGONS);
    }

    protected ACoord getFRUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        String k = this.getStoreGateKey();
        String sgKey = k != null ? k : "n/a";

        double dphi = 0.4;

        if(sgKey.contains("Kt6"))
            dphi = 0.6;
        if(sgKey.contains("Kt1"))
            dphi = 1.0;

        double s = parameterStore.get("Jet", "Scale").getD();
        double rhoMax = s*1500;
        double rhoMinus = parameterStore.get("Jet","Innerdrawradius").getD();
        double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
        if (0==s) return ACoord.NO_POLYLINES;

        for(int i=0; i<numDraw; i++)
        {
            int list = listdl[i];
            double phiPlus = phi[list] + dphi;
            double phiMinus = phi[list] - dphi;

            //if no composite particles increase jet size
            //Vector keys = (Vector) event.getCollections().get("CompositeParticle");
            //if (keys == null || !parameterStore.get("Data", "CompositeParticle").getStatus()) rhoMax=1000;//1800;

			double rhoPlus = rhoMinus + rhoMax;
			if(Math.abs(eT[list]) < maxEnergy) {
				if(parameterStore.get("Jet", "ScaleType").getI() == 1) //sq root
					rhoPlus = rhoMinus + rhoMax *  (Math.sqrt  (Math.abs(eT[list])/maxEnergy));
				if(parameterStore.get("Jet", "ScaleType").getI() == 0) // linear
					rhoPlus = rhoMinus + rhoMax *  (Math.abs(eT[list])/maxEnergy);
			}

            hv[0][i][0] = rhoMinus;
            hv[1][i][0] = Math.toDegrees(phiMinus);

            hv[0][i][1] = rhoMinus;
            hv[1][i][1] = Math.toDegrees(phiPlus);

            hv[0][i][2] = rhoPlus;
            hv[1][i][2] = Math.toDegrees(phiPlus);

            hv[0][i][3] = rhoPlus;
            hv[1][i][3] = Math.toDegrees(phiMinus);

            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.TRANSPARENT_POLYGONS).includePhiWrapAround("FR");    	
    }

    protected double Theta (double eta, int rSign)
    {
        // calculate theta based on the eta value
        double theta = Math.atan(Math.exp(-Math.abs(eta))) * 2.0;
        if ((eta > 0.) && (rSign == -1))
            theta = 2 * Math.PI - theta;
        else if ((eta < 0.) && (rSign == -1))
            theta += Math.PI;
        else if ((eta < 0.) && (rSign == 1))
            theta = Math.PI - theta;

        return theta;
    }

    protected ACoord getRZUser()
    {
        int coneSmooth = 20;
        double s = parameterStore.get("Jet", "Scale").getD();
        if (0==s) return ACoord.NO_POLYLINES;
        makeDrawList();
        double[][][] hv = new double[2][numDraw][(2*coneSmooth)];
        int[] index = new int[numDraw];

        double innerRadius = parameterStore.get("Jet","Innerdrawradius").getD();

        String k = this.getStoreGateKey();
        String sgKey = k != null ? k : "n/a";

        double deta = 0.4;

        if(sgKey.contains("Kt6"))
            deta = 0.6;
        if(sgKey.contains("Kt1"))
            deta = 1.0;


        for(int i=0; i<numDraw; i++)
        {
            int list = listdl[i];
            double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
            double phiDiff = Math.abs(phi[list]-phiMid);
            int rSign;
            if (phiDiff > Math.PI/2. && phiDiff <= 3*Math.PI/2.)
                rSign = -1;
            else
                rSign = 1;

            double theta = Theta(eta[list],rSign);
            double thetaPlus = Theta(eta[list] + deta, rSign);
            double thetaMinus = Theta(eta[list] - deta, rSign);

            // decide the region based on the theta value
            final byte TOP_BOTTOM = 0;
            final byte LEFT_RIGHT = 1;
            byte region = TOP_BOTTOM;
            // hard-coded value is based on the values in Geometry
            if(Math.abs(Math.tan(theta)) < 0.8)
                region = LEFT_RIGHT;

            double radiusMinus = 0.;

            switch(region)
            {
            case TOP_BOTTOM:
                radiusMinus = innerRadius / Math.abs(Math.sin(theta));
                break;
            case LEFT_RIGHT:
                radiusMinus = innerRadius / Math.abs(Math.cos(theta));
                break;
            }
            //compare to AAODData for rhoMax/Minus values
            double radiusMax = s*2500.;

            //if no composite particles increase jet size
            //Vector keys = (Vector) event.getCollections().get("CompositeParticle");
            //if (keys == null || !parameterStore.get("Data", "CompositeParticle").getStatus()) radiusMax+=200;

            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double radiusPlus = radiusMinus + radiusMax;
            if(Math.abs(eT[list] / Math.cos(AMath.lambda(eta[list]))) < maxEnergy)
            {
                if(parameterStore.get("Jet", "ScaleType").getI() == 1) //sq root
                    radiusPlus = radiusMinus + radiusMax *  (Math.sqrt  (Math.abs(eT[list] / Math.cos(AMath.lambda(eta[list])))/maxEnergy));
                if(parameterStore.get("Jet", "ScaleType").getI() == 0) // linear
                    radiusPlus = radiusMinus + radiusMax *((Math.abs(eT[list] / Math.cos(AMath.lambda(eta[list]))))/maxEnergy);
            }

            double vx = parameterStore.get("Event", "XVtx").getD();
            double vy = parameterStore.get("Event", "YVtx").getD();
            double vz = parameterStore.get("Event", "ZVtx").getD();

            double h_shift = vz;
            double v_shift = Math.sqrt(vx*vx + vy*vy);

            // the loop interpolates between the inner and outer edge of the jet to draw the arc
            // draw the outer arc
            for(int j=0; j<coneSmooth; j++){
                float f = ((float)j)/(coneSmooth-1);
                hv[0][i][j] = h_shift + (radiusPlus) * Math.cos(((1-f)*thetaPlus + (f)*thetaMinus));
                hv[1][i][j] = v_shift + (radiusPlus) * Math.sin(((1-f)*thetaPlus + (f)*thetaMinus));
            }

            // draw the inner arc
            for(int j=coneSmooth; j<(2*coneSmooth); j++){
                float f = ((float)j-coneSmooth)/(coneSmooth-1);
                hv[0][i][j] = h_shift + (radiusMinus) * Math.cos(((1-f)*thetaMinus + (f)*thetaPlus));
                hv[1][i][j] = v_shift + (radiusMinus) * Math.sin(((1-f)*thetaMinus + (f)*thetaPlus));
            }

            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.TRANSPARENT_POLYGONS);
    }
}
