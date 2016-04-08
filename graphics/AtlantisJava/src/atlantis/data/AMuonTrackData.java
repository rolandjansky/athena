package atlantis.data;

import atlantis.event.*;
import atlantis.canvas.AWindow;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionVP;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;


/**
 *
 * @author Eric Jansen
 */
public class AMuonTrackData extends ATrackData {

    private static ALogger logger = ALogger.getLogger(AMuonTrackData.class);
    
    protected float[] phi0;
    protected float[] pT;
    protected float[] cotTheta;
    protected int[] author;

    /** Creates a new instance of AMuonTrackData */
    AMuonTrackData(AHashMap p, AEvent e) throws AAtlantisException {
        super(p,e);
	String assocKey = getName() + getStoreGateKey();    
        if(p.get("numHits") != null) {
            int[] numHits = p.getIntArray("numHits");
            int[] hits = p.getIntArray("hits");

            event.getAssociationManager().add(new AAssociation(assocKey, "MDT", numHits, hits,event));
            event.getAssociationManager().add(new AAssociation(assocKey, "RPC", numHits, hits,event));
            event.getAssociationManager().add(new AAssociation(assocKey, "TGC", numHits, hits,event));
            event.getAssociationManager().add(new AAssociation(assocKey, "CSC", numHits, hits,event));
        }

        phi0 = p.getFloatArray("phi0");
        pT = p.getFloatArray("pt");
        cotTheta = p.getFloatArray("cotTheta");
	author = p.getUnsureIntArray("trackAuthor");
    }

    public String getParameterGroup() {
        return "MuonTrack";
    }

    public String getName() {
        return "MuonTrack";
    }

    public String getNameScreenName() {
        return "MuonTrack";
    }

    protected void finalizeConstruction() {
        super.finalizeConstruction();
/*
        // needs ZVTx
        h = new AHelix[numData];
        for(int i = 0; i < numData; ++i) {
            double p = AMath.getPFromPttL(pT[i], cotTheta[i]);
            double xVtx = parameterStore.get("Event", "XVtx").getD();
            double yVtx = parameterStore.get("Event", "YVtx").getD();
            double phiVtx = Math.atan2(yVtx, xVtx);
            double rho = Math.sqrt(xVtx * xVtx + yVtx * yVtx);
            double d0 = -rho * Math.sin(phi0[i] - phiVtx);
            // correct for average energy loss
            double pTCorrected = pT[i] * (p + 3.7) / p;

            h[i] = new AHelix((float)d0,
                    (float)(parameterStore.get("Event", "ZVtx").getD()),
                    (float)Math.toDegrees(phi0[i]), cotTheta[i], (float)pTCorrected);
            //calculatePhiCorrection(i);
        }
*/
    }

    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
        if(projection instanceof AProjectionVP 
                && parameterStore.get(PARAMETER_GROUP, "DrawnAs").getI() != DRAW_NEWHELIX)
        {
            ag.draw(window.calculateDisplay(getVPUser(window, projection)));
        }
        else
            super.draw(window, ag, projection);
    }

    protected ACoord getVPUser(AWindow window, AProjection projection)
    {
        makeDrawList();
        double[][][] hv = new double[2][2*numDraw][];
        int[] index = new int[2 * numDraw];
        int num = 0;
        for (int j=0; j<numDraw; j++) {
            int list = listdl[j];
            ADHelix dhelix = new ADHelix(h[list]);
            double s1 = dhelix.getAStart();
            double s2 = 179.;
            // hardwire projection.getMinRho() == 2. for now
            s1 = dhelix.intersectWithRadialCylinder(((AProjection2D)projection).getMinRho(), s1, s2);
            double sEnd = dhelix.intersectWithCylinder(true, AProjectionVP.getRhoVPlot(),
                    true, AProjectionVP.getZVPlot());
            s2 = Math.max(Math.min(s2, sEnd), s1);
            // if the whole helix is to be drawn (which are unusual
            // helices, shorten it a little to avoid wraparound problems
            if (s1 == 0. && s2 == 180.) s2 = 179.;

            if (parameterStore.get("VP", "ShortV").getStatus() && !parameterStore.get("Data", "S3D").getStatus()) {
                s1 = s2 - parameterStore.get("VP", "ShortV").getD() * (s2 - s1);
            }

            // by Zdenek:
            // if this condition is not entered, hv array containing coordinates
            // remains empty and Atlantis will crash with NullPointerException
            // in AWindow.calculateUser(ACoord user)
            // magic constants here (e.g. s2) need to be understood and this
            // method best reimplemented
            for(int sign=-1; sign<=1; sign+=2) {
                // ugly must change structure at some point
                AProjectionVP.sign = sign;
                ACoord pointsOnHelix;
                if (s2 > s1) {
                    pointsOnHelix = dhelix.drawHelix(window, (AProjection2D) projection, s1, s2);
                    hv[0][num] = pointsOnHelix.hv[0][0];
                    hv[1][num] = pointsOnHelix.hv[1][0];
                } else {
                    hv[0][num] = new double[0];
                    hv[1][num] = new double[0];
                }
                index[num] = list;
                num++;
            }
        }

        // by Zdenek:
        // condition to prevent NullPointerException in AWindow.calculateUser()
        // see previous comment ...
        // this condition should be removed completely 'bug in helix'
        // problem is understood
        ACoord coord = null;
        if(hv[0].length > 0 && hv[0][0] == null)
        {
            logger.warn("AMuonTrackData.getVPUser(): \"" + this.getName() +
                 ":" + this.getStoreGateKey() + "\" can't be drawn in eta-phi");
            coord = ACoord.NO_DATA;
        }
        else
        {
            coord = new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES);
        }
        return window.calculateUser(coord).includePhiWrapAround(projection.getName());
    }



    private float[] getEta()
    {
        float[] eta = new float[numData];
        if(polylinedTrack)
        {
            for(int i = 0; i < numData; i++)
            {
                if(h!=null && h[i]!=null) eta[i] = (float) h[i].eta();
            	else eta[i] = (float) AParameterUtilities.eta(z[i][0], rho[i][0]);
            }
        }
        else
        {
            for(int i = 0; i < numData; i++)
            {
                eta[i] = (float) h[i].eta();
            }

        }

        return eta;
    }



    protected void applyCuts() {
        cutIndex();
        cut("CutsInDet", "Pt", " |Pt|", pT);
	cut("CutsMuon", "Pt", " |Pt|", pT);
	cut("CutsInDet",  "trackAuthor", " Author", author);
        cutPhi(phi0);
        if(parameterStore.get("CutsATLAS", "CutEta").getStatus())
        {
            cutEta(getEta());
        }
    }
}
