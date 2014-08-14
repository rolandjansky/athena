package atlantis.data;

import java.awt.Color;
import java.awt.Point;
import java.awt.geom.Point2D;

import atlantis.canvas.AWindow;
import atlantis.globals.AGlobals;
import atlantis.graphics.AGraphics;
import atlantis.graphics.APickingGraphics2D;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjectionTrackResidual;

/**
 * Contains residual data for a single track
 * 
 * @author Qiang Lu
 */
public class ATrackResidualData
{
    public static String PULL_X = "P_X";
    public static String RESIDUAL_X = "R_X";
    
    private ATrackData track;
    private int trackIndex;
    private String showType = RESIDUAL_X;
    private int numPoints;
    private float[] pullLoc1;
    private float[] resLoc1;
    private float[] pullLoc2;
    private float[] resLoc2;
    private String[] detType;
    
    private double logMagnitudeMin;
    private int pickedIndex;
    private byte pickedGroup;    // 1 for loc1/pull1, 2 for loc2/pull2
    private double minDiff;   
    
    private static APar parameterStore = APar.instance();
    
    /**
     * @param theTrack track collection to which this residual belongs to
     * @param trackIndex index of the associated track
     * @param points number of residuals points
     * @param pullLoc1 residuals pull data 1
     * @param resLoc1 residuals data 1
     * @param pullLoc2 residuals pull data 2 (only for Pixel)
     * @param resLoc2 residuals data 2 (only for Pixel)
     * @param detType detector type of each residual
     */
    ATrackResidualData(ATrackData track, int trackIndex, int points, 
            float[] pullLoc1, float[] resLoc1, float[] pullLoc2, float[] resLoc2, 
            String[] detType)
    {
        this.track = track;
        this.trackIndex = trackIndex;
        numPoints = points;
        this.pullLoc1 = pullLoc1;
        this.resLoc1 = resLoc1;
        this.pullLoc2 = pullLoc2;
        this.resLoc2 = resLoc2;
        this.detType = detType;
    }

    /**
     * @return Returns the number of residual points.
     */
    public int getNumPoints()
    {
        return numPoints;
    }

    public float[] getPullLoc1()
    {
        return pullLoc1;
    }

    public float[] getResLoc1()
    {
        return resLoc1;
    }

    public float[] getPullLoc2()
    {
        return pullLoc2;
    }

    public float[] getResLoc2()
    {
        return resLoc2;
    }

    public void setLogMagnitudeMin(double logMagnitudeMin)
    {
        this.logMagnitudeMin = logMagnitudeMin;
    }
    
    public double getLogMagnitudeMin()
    {
        return logMagnitudeMin;
    }
    
    public void draw(AWindow window, AGraphics ag, AProjectionTrackResidual projection)
    {
        Color[] colorMap=AColorMap.getColors();
        
        // draw horizontal axis through 0
        double x1 = 0., y1 = 0., x2 = projection.getXLength(), y2 = 0.0;
        Point2D.Double from = window.calculateDisplay(x1, y1);
        Point2D.Double to = window.calculateDisplay(x2, y2);
        ag.setColor(colorMap[AColorMap.BL]);
        ag.drawLine(from.x, from.y, to.x, to.y);
        
        float[] temp1;
        AParameter showExtraPar = parameterStore.get("TrackResidual", "ShowExtra");
        if(projection.getResidualType().equals(RESIDUAL_X))
        {
            showType = RESIDUAL_X;
            temp1 = this.resLoc1;
            float[] temp2 = this.pullLoc1;  // needed when drawing error bar
            drawResidual(window, ag, temp1, temp2, colorMap, false);
            if(showExtraPar.getStatus())
            {
                temp1 = this.resLoc2;
                temp2 = this.pullLoc2;
                drawResidual(window, ag, temp1, temp2, colorMap, true);
            }
        }
        else
        {
            showType = PULL_X;
            temp1 = this.pullLoc1;
            drawResidual(window, ag, temp1, null, colorMap, false);
            if(showExtraPar.getStatus())
            {
                temp1 = this.pullLoc2;
                drawResidual(window, ag, temp1, null, colorMap, true);
            }
        }
                
        if(ag.getGraphics2D() instanceof APickingGraphics2D
                && parameterStore.get("Event", "PickingMode").getI() == APickingGraphics2D.PICK_HITS_AND_TRACKS)    // pick residula
        {
            APickingGraphics2D.mode = APickingGraphics2D.PICK_RESIDUAL;
            APickingGraphics2D.setPicked(this);
        }
    }

    public void drawResidual(AWindow window, AGraphics ag, float[] res, 
            float[] pull, Color[] colorMap, boolean drawExtra)
    {
        // every residual is drawn as a small square with 2 pixels * 2 pixels
        // N.B. when showType=PULL_X, res actually represents pull, and pull=null
        double[] h = new double[4];
        double[] v = new double[4];
        int pixelOffset = 1;
        
        Point clickedPoint = null;
        if(ag.getGraphics2D() instanceof APickingGraphics2D
                && parameterStore.get("Event", "PickingMode").getI() == APickingGraphics2D.PICK_HITS_AND_TRACKS)    // pick residual
        {
            clickedPoint = ((APickingGraphics2D)ag.getGraphics2D()).pickedPoint;
        }

        Color colValid = colorMap[AColorMap.RD];   //for backward compatibility
        Color colInvalid = colorMap[AColorMap.BK];
        for(int i=0; i<numPoints; i++)
        {   
            double y;
            if(res[i] == -99.0f)  // unavailable
            {
                ag.setColor(colInvalid);
                y = 0.0;
            }
            else
            {
                if(detType != null)
                {
                    String det = detType[i];
                    if("unident".equals(det))
                        ag.setColor(colorMap[AColorMap.GY]);
                    else
                    {
                        AParameter detColor = parameterStore.get("Det", det+"Fill");
                        if(detColor != null)
                            ag.setColor(colorMap[detColor.getI()]);
                        else
                            ag.setColor(colValid);
                    }
                }
                else    // for old files that don't have <tsosDetType>
                    ag.setColor(colValid);

                if(parameterStore.get("TrackResidual", "Scale").getI() == 0)    // linear
                    y = res[i];
                else    // logarithmic scale
                {
                    if(res[i] != 0.0f)
                        y = Math.log10(Math.abs(res[i])) - logMagnitudeMin;
                    else
                        y = res[i];
                    if(res[i] < 0.0f)
                        y = -y;
                }
            }

            double tempX, tempY;
            if(!drawExtra)
                tempX = window.calculateDisplay(i, y).x;
            else
                tempX = window.calculateDisplay(i+0.25, y).x;
            tempY = window.calculateDisplay(i, y).y;
            
            // when shape is a square, calculate the corners
            // x0, y0
            h[0] = tempX - pixelOffset;
            v[0] = tempY + pixelOffset;
            // x1, y1
            h[1] = tempX - pixelOffset;
            v[1] = tempY - pixelOffset;
            // x2, y2
            h[2] = tempX + pixelOffset;
            v[2] = tempY - pixelOffset;
            // x3, y3
            h[3] = tempX + pixelOffset;
            v[3] = tempY + pixelOffset;
           
            ag.fillPolygon(h, v, h.length);

            // draw error bar only for residual
            AParameter showErrorBarPar = parameterStore.get("TrackResidual", "ErrorBar");
            if(showType.equals(RESIDUAL_X) && showErrorBarPar.getStatus() && 
                    pull != null)
            {
                double error = 0.0;
                if(pull[i] != 0.0f && pull[i] != -99.0f && y != 0.0)
                    error = res[i] / pull[i];
                
                if(error != 0.0)    // draw the error bar by linear scale always
                {
                    double[][] vLine = new double[2][2];
                    vLine[0][0] = tempX;
                    vLine[0][1] = tempX;
                    vLine[1][0] = window.calculateDisplay(i, y+error).y;
                    vLine[1][1] = window.calculateDisplay(i, y-error).y;
                    ag.drawPolyline(vLine[0], vLine[1], vLine[0].length);   // verticle line
                    
                    double xMinus = tempX - 2 * pixelOffset;
                    double xPlus = tempX + 2 * pixelOffset;
                    double yTop = vLine[1][0];
                    double yBottom = vLine[1][1];
                    ag.drawLine(xMinus, yTop, xPlus, yTop);
                    ag.drawLine(xMinus, yBottom, xPlus, yBottom);
                }
            }
            
            // find the picked residual
            if(ag.getGraphics2D() instanceof APickingGraphics2D
                    && parameterStore.get("Event", "PickingMode").getI() == APickingGraphics2D.PICK_HITS_AND_TRACKS)    // pick residula
            {
                if(i==0 && !drawExtra)
                {
                    pickedIndex = i;
                    pickedGroup = 1;
                    minDiff = Math.abs(clickedPoint.x - tempX);
                    APickingGraphics2D.pickedH = tempX;
                    APickingGraphics2D.pickedV = tempY;
                }
                else
                {
                    double diff = Math.abs(clickedPoint.x - tempX);
                    if(minDiff > diff)
                    {
                        pickedIndex = i;
                        if(drawExtra)
                            pickedGroup = 2;
                        else
                            pickedGroup = 1;
                        minDiff = diff;
                        APickingGraphics2D.pickedH = tempX;
                        APickingGraphics2D.pickedV = tempY;
                    }
                }
            }
        }
    }
    
    public String getHitInfo()
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return track.getNameScreenName()+
    	" index: "+trackIndex+"\n storegate key: "+
    	(track.getStoreGateKey() == null ? "n/a" : track.getStoreGateKey());
    	
        StringBuffer msg = new StringBuffer(track.getNameScreenName() + " (index: ");
        msg.append(trackIndex);
        msg.append(")");
        msg.append("\n storegate key: ");
        msg.append(track.getStoreGateKey() == null ? "n/a" : track.getStoreGateKey());
        if(showType.equals(RESIDUAL_X))
        {
            msg.append("\n Residual" + pickedGroup + "(" + "index: " + pickedIndex + ") = ");
            if (pickedGroup == 1)
                msg.append((this.resLoc1[pickedIndex]==-99.0f)?"unavailable":this.resLoc1[pickedIndex] + "(mm)");
            else
                msg.append((this.resLoc2[pickedIndex]==-99.0f)?"unavailable":this.resLoc2[pickedIndex] + "(mm)");
        }
        else
        {
            msg.append("\n Pull" + pickedGroup + "(" + "index: " + pickedIndex + ") = ");
            if (pickedGroup == 1)
                msg.append((this.pullLoc1[pickedIndex]==-99.0f)?"unavailable":this.pullLoc1[pickedIndex] + "(mm)");
            else
                msg.append((this.pullLoc2[pickedIndex]==-99.0f)?"unavailable":this.pullLoc2[pickedIndex] + "(mm)");
        }
        
        return msg.toString();

    }
    
}
