package atlantis.data;

import atlantis.canvas.AWindow;
import atlantis.event.AEvent;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionVP;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

public abstract class AHelixAODData extends AAODData
{
    protected AHelix[] h = null;

    AHelixAODData(AHashMap p, AEvent e)
    {
        super(p,e);
        h = new AHelix[numData];
        float[] d0 = new float[numData];
        float[] z0 = new float[numData];
        float[] tl = new float[numData];
        for (int i = 0; i < numData; i++)
        {
            d0[i] = 0.0f;
            z0[i] = 0.0f;
            tl[i] = (float) AMath.tanLambda((double)eta[i]);
            h[i] = new AHelix(d0[i], z0[i], (float) Math.toDegrees(phi[i]), tl[i], pT[i]);
        }
    }

    private void drawHelix(AWindow window, AGraphics ag, AProjection2D projection)
    {
        int drawnAs = parameterStore.get("InDetTrack", "DrawnAs").getI();
        if (drawnAs == ATrackData.DRAW_NEWHELIX) {

           boolean showS3D = parameterStore.get("Data", "S3D").getStatus();
            AParameter shortV = parameterStore.get("VP", "ShortV");
            
            makeDrawList();
            double[][][] hv = new double[2][numDraw][];
            int[] index = new int[numDraw];

            for (int i=0; i<numDraw; i++) {
                int j = listdl[i];
                index[i] = j;

                double alphaMin = h[j].getAlphaExtrapolated(projection.getMinRho(), AHelix.TRACKER_LENGTH);
                double alphaMax = h[j].getAlphaExtrapolated(AProjectionVP.getRhoVPlot(), AHelix.TRACKER_LENGTH); 
                if (shortV.getStatus() && !showS3D) {
                    alphaMin = alphaMax - shortV.getD() * (alphaMax - alphaMin);
                }

                hv[0][i] = h[j].getEta(alphaMin, alphaMax);
                hv[1][i] = h[j].getPhi(alphaMin, alphaMax, true);
            }                

            ag.draw(window.calculateDisplay(new ACoord(hv, index, this, ACoord.POLYLINES)));

        } else {
            // borrow the implementation in for tracks
            // phi wrap around is done in user coordinates, so we convert to user coordinates,
            // do the phi wrap around and convert back to display coordinates
            ag.draw(window.calculateDisplay(window.calculateUser(
                    getVPDisplayHelices(window, projection)).includePhiWrapAround(projection.getName())
            ));
        }
    }
    
    // give back Drawable helices
    private ADHelix[] getHelices()
    {
        makeDrawList();
        ADHelix[] tempList = new ADHelix[numDraw];

        for (int i = 0; i < numDraw; i++)
            if (h != null && h[listdl[i]] != null)
            {
                tempList[i] = new ADHelix(h[listdl[i]]);
                if (tempList[i].getAStart() == tempList[i].getAEnd())
                {
                    tempList[i] = null;
                }
            }
            else
                tempList[i] = null;
        return tempList;
    }
    
    private ACoord getVPDisplayHelices(AWindow window, AProjection2D projection)
    {
        boolean drawApex = parameterStore.get("VP", "DrawApex").getStatus();
        ADHelix[] dhelix = getHelices();
        int size = 2 * dhelix.length;
        if (drawApex)
            size = 3 * dhelix.length;
        double[][][] hv = new double[2][size][0];
        int[] index = new int[size];
        int[] indexIn = getDrawList();
        int num = 0;

        for (int j = 0; j < dhelix.length; ++j)
            if (dhelix[j] != null)
            {
                double s1 = dhelix[j].getAStart();
                double s2 = 179.;

                s1 = dhelix[j].intersectWithRadialCylinder(projection.getMinRho(), s1, s2);
                double sEnd = dhelix[j].intersectWithCylinder(true, AProjectionVP.getRhoVPlot(), true, AProjectionVP.getZVPlot());

                s2 = Math.max(Math.min(s2, sEnd), s1);
                // if the whole helix is to be drawn (which are unusual
                // helices, shorten it a little to avoid wraparound problems
                if (s1 == 0. && s2 == 180.)
                    s2 = 179.;
                if (parameterStore.get("VP", "ShortV").getStatus() && !parameterStore.get("Data", "S3D").getStatus())
                    s1 = s2 - parameterStore.get("VP", "ShortV").getD() * (s2 - s1);
                if (s2 > s1)
                {
                    int signMin = -1;
                    int signMax = 1;
                    double h = 0;
                    double v = 0;
                    for (int sign = signMin; sign <= signMax; sign += 2)
                    {
                        // ugly must change structure at some point
                        AProjectionVP.sign = sign;
                        ACoord pointsOnHelix = dhelix[j].drawHelix(window, projection, s1, s2);
                        hv[0][num] = pointsOnHelix.hv[0][0];
                        hv[1][num] = pointsOnHelix.hv[1][0];
                        index[num] = indexIn[j];
                        h = hv[0][num][hv[0][num].length - 1];
                        v = hv[1][num][hv[0][num].length - 1];
                        num++;
                    }
                    if (drawApex)
                    {
                        int a = 3;
                        int b = 7;
                        hv[0][num] = new double[] { h - a, h + a, h, h, h - a, h + a };
                        hv[1][num] = new double[] { v - b, v - b, v - b, v + b, v + b, v + b };
                        index[num] = indexIn[j];
                        num++;
                    }
                }
            }
        return new ACoord(hv, index, this, ACoord.POLYLINES);
    }
    
    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
        if(projection instanceof AProjectionVP)
        {
                // since no d0 and z0 data is provided for AOD Electron and Muon
                // from event file, they are set to 0 manually, which is not
                // compatible with the primary vertex calculated in Atlantis, and
                // it will cause a wide and bizarre shape of 'v' drawn in V-Plot.
                // So the temparary solution is to set primary vertex to (0,0,0)
                // before drawing AOD Electron and Muon in V-Plot, and then restore
                // their values after.
                double[] primaryVtx=event.getPrimaryVertex();
                parameterStore.get("Event", "XVtx").setD(0.0);
                parameterStore.get("Event", "YVtx").setD(0.0);
                parameterStore.get("Event", "ZVtx").setD(0.0);
                // super.draw(window, ag, projection);
                drawHelix(window, ag, projection);
                parameterStore.get("Event", "XVtx").setD(primaryVtx[0]);
                parameterStore.get("Event", "YVtx").setD(primaryVtx[1]);
                parameterStore.get("Event", "ZVtx").setD(primaryVtx[2]);
            }
        else
        {
            super.draw(window, ag, projection);
        }
    }

}
