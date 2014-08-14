package atlantis.projection;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.geom.Point2D;

import atlantis.canvas.AWindow;
import atlantis.data.ADHelix;
import atlantis.data.ATrackResidualData;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.parameters.APar;
import atlantis.utils.AMath;

public class AProjectionTrackResidual extends AProjection2D
{
    private ATrackResidualData residual;
    private String residualType;
    private double xLength;
    private double yLength = 1.0;
    
    /**
     * @param residual The residual to set.
     */
    public void setResidual(ATrackResidualData residual)
    {
        this.residual = residual;
    }

    public ATrackResidualData getResidual()
    {
        return residual;
    }
    
    public void setResidualType(String type)
    {
        residualType = type;
    }

    public String getResidualType()
    {
        return residualType;
    }

    public String getName()
    {
        return "TrackResidual";
    }

    public String getXLabel()
    {
        return "Index";
    }

    public String getYLabel()
    {
        return "Res";
    }

    public String getXUnits()
    {
        return "";
    }

    public String getYUnits()
    {
        return "(mm)";
    }

    public double getXLength()
    {
        return xLength;
    }

    public double getYLength()
    {
        return yLength;
    }

    public Point2D.Double getCenter()
    {
        return new Point2D.Double(0.0, 0.0);
    }

    // not useful for this projection, only put here to be compatible with
    // other code
    public Point2D.Double[] calculateNoZoomCorners(Dimension wSize)
    {
        Point2D.Double[] corners=new Point2D.Double[3];
        
        corners[0]=new Point2D.Double(0., 2.);
        corners[1]=new Point2D.Double(100., 2.);
        corners[2]=new Point2D.Double(100., -2.);
        return corners;
    }

    public Point2D.Double[] calculateNoZoomCorners()
    {
        float[] dataArray;
        if(residualType.equals(ATrackResidualData.RESIDUAL_X))
            dataArray = residual.getResLoc1();
        else
            dataArray = residual.getPullLoc1();
        
        xLength = dataArray.length;
        if(parameterStore.get(getName(), "Scale").getI() == 0)    // linear
        {
            yLength = (AMath.maxAbsInArray(dataArray) + 0.1) * 2;
        }
        else    // logarithmic
        {
            float[] dataArrayCopy = new float[dataArray.length];
            for(int i=0; i<dataArrayCopy.length; ++i)
            {
                if(dataArray[i] == -99.0f)
                    dataArrayCopy[i] = 0.0f;
                else
                    dataArrayCopy[i] = dataArray[i];
            }
            double minValue = (double) AMath.minAbsInArray(dataArrayCopy);
            double maxValue = (double) AMath.maxAbsInArray(dataArrayCopy);
            double magnitude;
            
            // all data is either invalid or 0
            if(maxValue == 0.0 && minValue == 0.0)
            {
                magnitude = 1;
                residual.setLogMagnitudeMin(-1.0);
            }
            else if (minValue == 0.0)
            {
                double minNonZeroValue = (double) AMath.minNonZeroAbsInArray(dataArrayCopy);
                magnitude = Math.floor(Math.log10(maxValue)) - 
                    Math.floor(Math.log10(minNonZeroValue))+ 1.0;
                residual.setLogMagnitudeMin(Math.floor(Math.log10(minNonZeroValue)));
            }
            else
            {
                magnitude = Math.floor(Math.log10(maxValue)) -
                    Math.floor(Math.log10(minValue))+ 1.0;
                residual.setLogMagnitudeMin(Math.floor(Math.log10(minValue)));
            }
            yLength = magnitude * 2;
        }
        Point2D.Double[] corners = new Point2D.Double[3];
        
        corners[0] = new Point2D.Double(0., yLength/2);
        corners[1] = new Point2D.Double(xLength, yLength/2);
        corners[2] = new Point2D.Double(xLength, -yLength/2);
        return corners;
    }

    public ACoord getUserPoint(ADHelix dH, double s)
    {
        return ACoord.NO_DATA;
    }

    public void paint(AWindow window, Graphics g) 
    {
        AGraphics ag = AGraphics.makeAGraphics(g);
        
        fillBackground(window, ag);
        if(residual != null)
            residual.draw(window, ag, this);
    }
    
}
