package atlantis.canvas;

import atlantis.graphics.AAbstractGraphics2D;
import atlantis.graphics.ACursorFactory;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.Rectangle;
import java.awt.Stroke;
import java.awt.Image;
import java.awt.geom.AffineTransform;
import java.awt.geom.NoninvertibleTransformException;
import java.awt.geom.Point2D;

import javax.swing.border.Border;

import atlantis.gui.APreferencesControl;
import atlantis.parameters.AEnumeratorParameter;
import atlantis.parameters.ALinkParameter;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.projection.AProjectionEventInfo;
import atlantis.projection.AProjectionNGE;
import atlantis.projection.AProjectionTrackResidual;
import atlantis.utils.AMath;
import java.awt.RenderingHints;

/**
 * This class implements a Border in which the left and bottom sides contain
 * numerical scales.
 *
 * @author Charles Loomis, Sebastian Boeser
 */
public class AScaleBorder implements Border
{
	// Constants giving horizontal and vertical index of the data arrays.
    final static private int HORIZONTAL = 0;
    final static private int VERTICAL = 1;

    /**
     * Constant describing a transform in which the transformed x and y axes are
     * parallel (or antiparallel) to the original axes.
     */
    final static public int TYPE_PARALLEL_TRANSFORM = 0;

    /**
     * Constant describing a transform in which the transformed x and y axes are
     * parallel (or antiparallel) to the original y and x axes, respectively.
     */
    final static public int TYPE_SWITCHED_TRANSFORM = 1;

    /**
     * Constant describing a transform in which the transformed x-axis is
     * parallel (or antiparallel) to the original x-axis and the transformed
     * y-axis forms a non-zero angle with the original one.
     */
    final static public int TYPE_X_SKEW_TRANSFORM = 2;

    /**
     * Constant describing a transform in which the transformed y-axis is
     * parallel (or antiparallel) to the original y-axis and the transformed
     * x-axis forms a non-zero angle with the original one.
     */
    final static public int TYPE_Y_SKEW_TRANSFORM = 3;

    /**
     * Constant describing a transform in which the transformed x-axis is
     * parallel (or antiparallel) to the original y-axis and the transformed
     * y-axis forms a non-zero angle with the original x-axis.
     */
    final static public int TYPE_SWITCHED_X_SKEW_TRANSFORM = 4;

    /**
     * Constant describing a transform in which the transformed y-axis is
     * parallel (or antiparallel) to the original x-axis and the transformed
     * x-axis forms a non-zero angle with the original y-axis.
     */
    final static public int TYPE_SWITCHED_Y_SKEW_TRANSFORM = 5;

    /**
     * Constant describing a transform which does not fall into one of the other
     * categories.
     */
    final static public int TYPE_GENERAL_TRANSFORM = 6;

    /**
     * Constant indicating that a string should be aligned vertically with the
     * baseline of the text. This is the default in drawString calls which do
     * not specify an alignment.
     */
    public static final int TEXT_BASELINE = 0;

    /**
     * Constant indicating that a string should be aligned vertically with the
     * top of the text.
     */
    public static final int TEXT_TOP = 1;

    /**
     * Constant indicating that a string should be aligned vertically with the
     * bottom of the text.
     */
    public static final int TEXT_BOTTOM = 3;

    /**
     * Constant indicating that a string should be aligned by the center. This
     * is used for both horizontal and vertical alignment.
     */
    public static final int TEXT_CENTER = 2;

    /**
     * Constant indicating that a string should be aligned horizontally with the
     * left side of the text. This is the default for drawString calls which do
     * not specify an alignment.
     */
    public static final int TEXT_LEFT = 1;

    /**
     * Constant indicating that the string should be aligned horizontally with
     * the right side of the text.
     */
    public static final int TEXT_RIGHT = 3;
    
    private AScale horizontalScale; // holds tick marks and labels
    private AScale verticalScale;

    /**
     * String giving the axis labels.
     */
    private String[] axisLabels = new String[2];

    /**
     * String giving the axis units.
     */
    private String[] axisUnits = new String[2];

    /**
     * The initial font size for labeling.
     */
    private int fontSize = 12;

    /**
     * The initial font to use.
     */
    private Font labelFont = new Font("SansSerif", Font.BOLD, 12);

    /**
     * The width of the line used for the secondary tick marks.
     */
    final static private Stroke thinStroke = new BasicStroke(1.f);

    /**
     * The width of the line used for the primary tick marks.
     */
    final static private Stroke thickStroke = new BasicStroke(2.f);

    /**
     * An array to hold temporary point values for transformation.
     */
    private double[] axisPts = new double[6];

    /**
     * Flag to indicate that the scale has changed and that it needs to be
     * redrawn.
     */
    private boolean scaleChanged;


    /**
     * The current width of the horizontal scale.
     */
    private int currentWidth;

    /**
     * The current width of the vertical scale.
     */
    private int currentHeight;

    /**
     * Minimum value on the horizontal axis.
     */
    private double minHoriz;

    /**
     * Maximum value of the horizontal axis.
     */
    private double maxHoriz;

    /**
     * Minimum value of the vertical axis.
     */
    private double minVert;

    /**
     * Maximum value on the vertical axis.
     */
    private double maxVert;

    /**
     * The calculated Insets for this border.
     */
    private Insets insets;

    private boolean selected;
    
    /** 
     * Wether to draw the scales for this window or not
     */
    private boolean drawScaleBorder;
    
    /** 
     * Wether to draw the scales for this window or not
     * independant of the projection being displayed
     */
    private boolean drawScaleBorder_preference;
    
    
    /**
     * The window this scale is drawn in.
     */
    private final AWindow aWindow;

    /**
     * Constructs a AScaleBorder
     * @param theWindow the window this scale belongs to
     */
    public AScaleBorder(AWindow theWindow)
    {
        aWindow = theWindow;

        // Give default values for the size of the axes.
        minHoriz = 0.;
        maxHoriz = 0.;
        minVert = 0.;
        maxVert = 0.;

        // Make the new insets.
        resetInsets();

        // Set the current width and height.
        currentWidth = 0;
        currentHeight = 0;
        scaleChanged = false;

        selected = false;
        drawScaleBorder = true;
        drawScaleBorder_preference = true;
    }

    public void forceDraw()
    {
        scaleChanged = true;
    }

    public void toggleScale()
    {
        drawScaleBorder_preference = !drawScaleBorder_preference;
        //only actually toggle scale if on a window with scale
        AProjection p = aWindow.getProjection();

        if(p instanceof AProjectionLegoPlot || p instanceof AProjectionNGE || p instanceof AProjectionEventInfo)
            return;
       
        drawScaleBorder = !drawScaleBorder;
        scaleChanged = true;
        resetInsets();
    }

    public void hideScale()
    {
        //upon window change of projection check if scales need to hide or not
        AProjection p = aWindow.getProjection();	
        	
        // Check global 'Hide Scales' option (in preferences menu)
        // If hiding scales, set all scales to false
        //
        if(APreferencesControl.getHideScalesMenuItem())
        {
        		drawScaleBorder=false;
        		resetInsets();
        }
        else 
        	if(p instanceof AProjectionLegoPlot || p instanceof AProjectionNGE || p instanceof AProjectionEventInfo){
            if(drawScaleBorder){
                drawScaleBorder=false;
                resetInsets();
            }
        }else if(drawScaleBorder!=drawScaleBorder_preference){
            drawScaleBorder=drawScaleBorder_preference;
            resetInsets();
        }
    }
    
    public boolean getScaleStatus()
    {
        return drawScaleBorder_preference;
    }

    /**
     * Set the font for the labels.
     */
    public void setLabelFont(Font labelFont)
    {
        this.labelFont = labelFont;
        fontSize = labelFont.getSize();
        resetInsets();
    }

    public void setSelected()
    {
        selected = true;
    }

    public void setDeselected()
    {
        selected = false;
    }

    /**
     * Get the current font for the labels.
     */
    public Font getLabelFont()
    {
        return labelFont;
    }

    /**
     * Set the horizontal and vertical limits for the scales.
     */
    public void setLimits(double minHoriz, double maxHoriz, double minVert, double maxVert)
    {
        this.minHoriz = minHoriz;
        this.maxHoriz = maxHoriz;
        this.minVert = minVert;
        this.maxVert = maxVert;
        scaleChanged = true;
    }

    /**
     * Set the axis labels.
     */
    public void setAxisLabels(String horizontalLabel, String verticalLabel)
    {
        axisLabels[HORIZONTAL] = horizontalLabel;
        axisLabels[VERTICAL] = verticalLabel;
    }

    public String getHorizontalAxisLabel()
    {
        return axisLabels[HORIZONTAL];
    }

    public String getVerticalAxisLabel()
    {
        return axisLabels[VERTICAL];
    }

    /**
     * Set the axis units.
     */
    public void setAxisUnits(String horizontalUnits, String verticalUnits)
    {
        if (horizontalUnits != null)
        {
            axisUnits[HORIZONTAL] = horizontalUnits;
        }
        else
        {
            axisUnits[HORIZONTAL] = "";
        }

        if (verticalUnits != null)
        {
            axisUnits[VERTICAL] = verticalUnits;
        }
        else
        {
            axisUnits[VERTICAL] = "";
        }
    }

    /**
     * Returns the insets of this border.
     */
    @Override
    public Insets getBorderInsets(Component c)
    {
        return (Insets) insets.clone();
    }

    /**
     * Returns whether or not the border is opaque. This always returns true.
     */
    @Override
    public boolean isBorderOpaque() {return true;}
    
    /**
     * Paints the border and window decorations for the specified component with the specified graphics
     * context, position, and size.
     */
    @Override
    public void paintBorder(Component c, Graphics g, int x, int y, int width, int height)
    {
    	//Get the graphics context
    	Graphics2D vg = (Graphics2D) g;
    	//Sanity check
    	if (vg==null) return;

    	//Select background and frame colors
    	Color bg = AWindow.BORDER_BACKGROUND_COLOR ;

    	//Paint the border frame
    	paintBorderFrame(vg,bg,width,height);

    	//If drawing the border draw scales
    	if (drawScaleBorder)
    	{
    		//Now check which projection we have
    		AProjection p = aWindow.getProjection();

    		// Set the flag indicating that the scales must be remade.
    		scaleChanged = (scaleChanged || width != currentWidth || height != currentHeight);

    		//set the Scales 
    		setScales(p.getXLabel(), p.getYLabel(), p.getXUnits(), p.getYUnits(), makeTransform(aWindow.getUserCorners()), aWindow.getWidth(), aWindow.getHeight());

    		//Paint the grid if there is one for this projection
    		paintGrid(vg,p,width,height);

    		//Paint the scales themselves
    		paintScales(vg,p,width,height);

    		// Reset the current width and height.
    		currentWidth = width;
    		currentHeight = height;
    		scaleChanged = false;
    	}

    	//Check whether Window Title needs to be drawn
    	if(APreferencesControl.getWindowTitleMenuItem()) {
    		paintWindowTitle(g, width, height,true);
    	}
    	
    	//Check whether FishEye Indicator needs to be drawn
    	if(APreferencesControl.getFisheyeIndicatorMenuItem()) {
    		paintFishEyeIndicator(g, false, width, height);
    	}
    }

    /**
     * Paints the part of the border that appears when the window is printed
     */
    void printBorder(Component c, Graphics g, int x, int y, int width, int height)
    {
    	paintBorder(c,g,x,y,width,height);
    }

 
    /**
     * Draw the part of the scales that shows up in printing
     */
    private void paintScales(Graphics2D g, AProjection p, int width, int height){
    	APar parameterStore = APar.instance();

        // Make our own graphics contex, so we don't cause trouble elsewhere
        Graphics2D svg = (Graphics2D) g.create();
        
        Color foregroundColour = selected ? AWindow.BORDER_SELECTED_FOREGROUND_COLOR : AWindow.BORDER_FOREGROUND_COLOR;
     
        // Get the horizontal scale.
        if (scaleChanged)
        {
        	int scaleLength = width - (insets.left + insets.right);
        	horizontalScale = AScale.calculateScale(minHoriz, maxHoriz, scaleLength, 3, 7, AScale.RIGHT_TICKS,
                    HORIZONTAL, p);
        }

        svg.translate(insets.left, height - insets.bottom + 2);

        svg.setColor(foregroundColour);
        svg.setFont(new Font("SansSerif", Font.BOLD, 12));

        // Draw the primary and secondary tick marks.
        svg.setStroke(thickStroke);
        svg.draw(horizontalScale.primaryTicks);
        svg.setStroke(thinStroke);
        svg.draw(horizontalScale.secondaryTicks);

        ScaleUnits hScaleUnits = new ScaleUnits(axisUnits[HORIZONTAL],minHoriz,maxHoriz);
        double factorH = hScaleUnits.scaleFactor;
        String unitsH = hScaleUnits.units;

        ScaleUnits vScaleUnits = new ScaleUnits(axisUnits[VERTICAL],minVert,maxVert);
        double factorV = vScaleUnits.scaleFactor;
        String unitsV = vScaleUnits.units;

        // Paint the tick labels.
        int numLabelsH = horizontalScale.labelValues.length;
        for (int i = 0; i < numLabelsH; i++)
        {
        	double valueScaled = horizontalScale.labelValues[i] * factorH;
        	int nDecPlaces = getNumDecimalPlaces(valueScaled);
        	String label = String.format("%."+nDecPlaces+"f", valueScaled);
        	drawString(svg, label,
        			(float) horizontalScale.labelPositions[i], AScale.getPrimaryTickSize() + 0.2f * fontSize, TEXT_CENTER, TEXT_TOP, width - insets.left - insets.right);
        }

        // Paint the axis label.
        float axisPositionX = (float) horizontalScale.getTitlePosition();
        drawString(svg, axisLabels[HORIZONTAL] + " " + unitsH, axisPositionX, AScale.getPrimaryTickSize() + 0.2f * fontSize, TEXT_CENTER, TEXT_TOP, width - insets.left - insets.right);

        // End this context.
        svg.dispose();

        // Now create a new graphics context for the vertical axis.
        svg = (Graphics2D) (g.create());

        // Create the vertical scale.
        if (scaleChanged)
        {
        	int scaleLength = height - (insets.top + insets.bottom);
            verticalScale = AScale.calculateScale(minVert, maxVert, scaleLength, 3, 7, AScale.LEFT_TICKS,
                    VERTICAL, p);
        }

        svg.translate(insets.left - 2, height - insets.bottom);
        svg.rotate(-Math.PI / 2.);

        svg.setColor(foregroundColour);
        svg.setFont(new Font("SansSerif", Font.BOLD, 12));

        // Draw the primary and secondary tick marks.
        svg.setStroke(thickStroke);
        svg.draw(verticalScale.primaryTicks);
        svg.setStroke(thinStroke);
        svg.draw(verticalScale.secondaryTicks);

        // Paint the tick labels.
        int numLabelsV = verticalScale.labelValues.length;
        for (int i = 0; i < numLabelsV; i++)
        {
        	double valueScaled = verticalScale.labelValues[i] * factorV;
        	int nDecPlaces = getNumDecimalPlaces(valueScaled);
        	String label = String.format("%."+nDecPlaces+"f", valueScaled);
        	if(p instanceof AProjectionTrackResidual && parameterStore.get("TrackResidual", "Scale").getI() == 1)
        	{
        		if(!label.equals("0"))
        		{
        			long tmp = Math.round(((AProjectionTrackResidual)p).getResidual().getLogMagnitudeMin() + Math.abs(Double.parseDouble(label)));
        			if(label.indexOf("-") == -1) //positive
        				label = "1E" + tmp;
        			else
        				label = "-1E" + tmp;
        		}
        	}
        	drawString(svg, label, (float) verticalScale.labelPositions[i], -(AScale.getPrimaryTickSize() + 0.2f * fontSize), TEXT_CENTER, TEXT_BOTTOM, height - insets.top - insets.bottom);
        }

        // Paint the axis label.
        float axisPositionY = (float) verticalScale.getTitlePosition();
        drawString(svg, axisLabels[VERTICAL] + " " + unitsV, axisPositionY, -(AScale.getPrimaryTickSize() + 0.2f * fontSize), TEXT_CENTER, TEXT_BOTTOM, height - insets.top - insets.bottom);

        // End this context.
        svg.dispose();

    }
    
    /**
     * Calculate number of decimal places needed in formatting number.
     * @param x number to format
     * @return number of decimal places needed
     */
    static int getNumDecimalPlaces(double x) {
    	int m = - (int) Math.floor(Math.log10(Math.abs(x)));
		return Math.max(0, m);
	}

	/** 
     * Draw the grid for the V-Plot
     **/
    private void paintGrid(Graphics2D g, AProjection p, int width, int height){
        
        // Make our own graphics contex, so we don't cause trouble elsewhere
        Graphics2D svg = (Graphics2D) g.create();
 
        // next part draws the grid for the V-plot
        if (p.getName().equals("VP"))
        {
            //Set color for grid lines
            svg.setColor(Color.LIGHT_GRAY);
            
            // eta grid projections values - these particular values
            // will be useful for calo people to see the eta grid lines at
            float[] etaGridPositions = { -4.9f, -3.2f, -2.5f, -1.375f, -0.8f,
                                         0.8f, 1.375f, 2.5f, 3.2f, 4.9f };

            for(int i = 0; i < etaGridPositions.length; i++)
            {
                if (etaGridPositions[i] > minHoriz && etaGridPositions[i] < maxHoriz) {
                    Point2D.Double startp = aWindow.calculateDisplay(etaGridPositions[i],minVert);
                    Point2D.Double endp = aWindow.calculateDisplay(etaGridPositions[i],maxVert);
                    svg.drawLine((int)startp.x,(int)startp.y,(int)endp.x,(int)endp.y);
                }
            }

            for (int i = (int)Math.ceil(minVert/90.)*90; i <= maxVert; i += 90)
            {
                Point2D.Double startp = aWindow.calculateDisplay(minHoriz,i);
                Point2D.Double endp = aWindow.calculateDisplay(maxHoriz,i);
                if (Math.abs(i) % 360 == 0) {
                    svg.setColor(Color.WHITE);
                    svg.setStroke(thickStroke);
                } else {
                    svg.setColor(Color.LIGHT_GRAY);
                    svg.setStroke(thinStroke);
                }
                svg.drawLine((int)startp.x,(int)startp.y,(int)endp.x,(int)endp.y);
            }
        }
        
        svg.dispose();
        
    }
    
    /**
     * This bit of the frame that is drawn when printing (appart from scales)
     */
    private void printBorderFrame(Graphics2D g, Color bg, int width, int height){
   
        // Make our own graphics contex, so we don't cause trouble elsewhere
        Graphics2D svg = (Graphics2D) g.create();
 
        svg.setColor(bg);
        //simply draw rectangles all around window
        svg.fillRect(0, 0, width, insets.top);
        // Added -insets.bottom because sometimes this stuff is transparent -Adam
        svg.fillRect(0, 0, insets.left, height - insets.bottom);
        svg.fillRect(width - insets.right, 0, insets.right, height);
        svg.fillRect(0, height - insets.bottom, width, insets.bottom);
        
        svg.dispose();
    }
    
    /** 
     * Paint the Frame of the border with the given colors
     */
    private void paintBorderFrame(Graphics2D g, Color bg, int width, int height){        
        //Draw the frame same as for printing
        printBorderFrame(g,bg,width,height);
        
       // Make our own graphics contex, so we don't cause trouble elsewhere
        Graphics2D svg = (Graphics2D) g.create();
        
        // Add a bit of GUI-look-and-feel frame
        if (selected){
            //white triangle bottom left corner - clears any background colour
            svg.setColor(AWindow.BORDER_SELECTED_FRAMELIGHT_COLOR);
            int xp[]={0,0,11,0};
            int yp[]={height-12,height,height,height-12};
            svg.fillPolygon(xp,yp,4);          
            //blue border around window
            svg.setColor(AWindow.BORDER_SELECTED_FRAME_COLOR);
            svg.drawLine(0, 0, width-1, 0);
            svg.drawLine(0, 0, 0, height-10);
            svg.drawLine(width-1, 0, width-1, height-1);
            svg.drawLine(10, height-1, width-1, height-1);
            svg.drawLine(0, height-10, 9, height-1);
            //white inner border
            svg.setColor(AWindow.BORDER_SELECTED_FRAMELIGHT_COLOR);
            svg.drawLine(10, height-2, width-2, height-2);
            svg.drawLine(1, 1, 1, height-10);
            //grey triangle bottom left corner
            svg.setColor(AWindow.BORDER_BACKGROUND_COLOR);
            xp[2]=8;
            yp[0]=height-9;
            yp[3]=height-9;
            svg.fillPolygon(xp,yp,4);
        }
        
        svg.dispose();
    }
    
    /**
     * Paint a small box at the top of each window to describe what is on the window
     */
    private void paintWindowTitle(Graphics g, int width, int height, boolean highlight)
    {
    	APar parameterStore = APar.instance();

        // Make our own graphics contex, so we don't cause trouble elsewhere
        Graphics2D svg = (Graphics2D) g.create();
        //Get Mode parameter
        String s=aWindow.getProjection().getName();
        parameterStore.selectWindowParameters(aWindow.getName());
        AParameter param=parameterStore.get(s, "Mode");
        int mode=param.getI();
        s=aWindow.getProjection().getScreenName();
        s+=" Projection";
        //Add mode text if not on standard view 
        if (param != null)
        {
            if(mode != 0)
            {
                s+=" viewing: ";
                if(param instanceof ALinkParameter){
                    s+=((ALinkParameter) param).getCurrentText();
                }else{
                    s+=((AEnumeratorParameter) param).getCurrentText();
                }
            }
        }
        parameterStore.restoreWindowParameters();
        //Offset if indicator to window frame
        int Offset=2;
        
        int boxsize,y;
        // get the width of the text
        boxsize = svg.getFontMetrics().stringWidth(s)+Offset*2;        
        //Get y position from offset
        y = Offset+svg.getFontMetrics().getHeight();
        
        //Get the x position from width of window
        int x = insets.left + (int) Math.round(((width-insets.left-boxsize)/2.0));
        //Draw background box and set text color
        //highlighting the active window if desired
        if(selected&&highlight)
        {
            //If selected and not printing eps 
            //add a bit of GUI-look-and-feel frame
            svg.setColor(AWindow.BORDER_SELECTED_BACKGROUND_COLOR);
            svg.fillRect(x-2, 1, boxsize+4, y+1);
            svg.setColor(AWindow.BORDER_SELECTED_FRAME_COLOR);
            svg.fillRect(x-1, 0, boxsize+2, y+1);
            svg.setColor(AWindow.BORDER_SELECTED_FRAMELIGHT_COLOR);
            svg.fillRect(x, 1, boxsize, y-1);
            svg.setColor(AWindow.BORDER_SELECTED_BACKGROUND_COLOR);
            svg.fillRect(x+1, 1, boxsize-1, y-2);
            svg.setColor(AWindow.BORDER_SELECTED_FOREGROUND_COLOR);
        }
        else
        {
            //If not selected draw background box
            svg.setColor(AWindow.BORDER_BACKGROUND_COLOR);
            svg.fillRect(x-2, 0, boxsize+4, y+2);
            svg.setColor(AWindow.BORDER_FOREGROUND_COLOR);
        }
        //Draw text
        svg.drawString(s, x+Offset, y-3);
        //cleanup
        svg.dispose();
    }

    /**
     *  Paint a small FishEye Indicator if FishEye projection is active.
     *  At the moment, the indicator is drawn in the top right corner of the canvas.
     *  One might as well put it in on the scale. Eventually, if the scales shows
     *  proper fisheyed coordinates, it might not be needed anymore at all.
     */
    private void paintFishEyeIndicator(Graphics g, boolean forceText, int width, int height){
      APar parameterStore = APar.instance();

      //Probe for fishEye status
      boolean fishEyeStatus=false;
      
      //Check if FishEye is on for this particular window
      parameterStore.selectWindowParameters(aWindow.getName());
      
      //Get FishEye parameter
      AParameter fishEyePar = parameterStore.get(aWindow.getProjection().getName(), "FishEye");

      //Check if there is a fish-eye parameter for this projection, and get status (enabled or not)
      if (fishEyePar != null) fishEyeStatus=fishEyePar.getStatus();
      
      //Restore to active window
      parameterStore.restoreWindowParameters();

      // Now draw indicator if fisheye was on
      if (fishEyeStatus){
 
        // Make our own graphics contex, so we don't cause trouble elsewhere
        Graphics2D svg = (Graphics2D) g.create();            
        //Offset if indicator to window frame
        int Offset=4;
        //Check if we have custom cursors - only then use indicator image
        if (!forceText ){
          //FishEye indicator image is provided by ACursorFactory as it is the same
          //as the cursor used in FishEye transformation.
          Image IndicatorImg = ACursorFactory.getInstance().getFishEyeIndicator();
          //Get the x position from width of window and width of image
          int x = width-Offset-IndicatorImg.getWidth(null);
          //Get y position from offset
          int y = Offset;
          //Draw an fisheye indicator in top right corner with some space
          svg.drawImage(IndicatorImg,x,y,null);
        } else {
          int x,y;
          //Get the x position from width of window and width of image
          x = width-Offset-svg.getFontMetrics().stringWidth("FishEye");
          //Get y position from offset
          y = Offset+svg.getFontMetrics().getHeight();
          //Set color to red
          svg.setColor(Color.red);
          //Use small label to indicate fisheye
          svg.drawString("FishEye",x,y);
   
        }
        //cleanup
        svg.dispose();
      }
    }

    /**
     * Recalculate the insets based on the current font size.
     */
    private void resetInsets()
    {
        int lb = (int) (2 + 2 + AScale.getPrimaryTickSize() + 1.5 * fontSize);
        // int tr = (int) (2+2+AScale.getPrimaryTickSize());
        int tr = 2;

        if (drawScaleBorder)
            insets = new Insets(tr, lb, lb, tr);
        else
            insets = new Insets(tr, tr, tr, tr);
    }

    /**
     * Set the scale labels taking into account the given linear transformation.
     */
    public void setScales(String horizLabel, String vertLabel, String horizUnits, String vertUnits, AffineTransform transform, int panelWidth, int panelHeight)
    {

        // Determine the type of the tranform.
        int type = classifyTransform(transform);

        switch (type)
        {
            case TYPE_PARALLEL_TRANSFORM:
                setAxisLabels(horizLabel, vertLabel);
                setAxisUnits(horizUnits, vertUnits);
                break;

            case TYPE_SWITCHED_TRANSFORM:
                setAxisLabels(vertLabel, horizLabel);
                setAxisUnits(vertUnits, horizUnits);
                break;

            case TYPE_Y_SKEW_TRANSFORM:
                setAxisLabels(horizLabel + AMath.PRIME, AMath.DELTA + vertLabel);
                if (horizUnits.equals(vertUnits))
                {
                    setAxisUnits(horizUnits, vertUnits);
                }
                else
                {
                    setAxisUnits(horizUnits + AMath.DOT + vertUnits, vertUnits);
                }
                break;

            case TYPE_X_SKEW_TRANSFORM:
                setAxisLabels(AMath.DELTA + horizLabel, vertLabel + AMath.PRIME);
                if (vertUnits.equals(horizUnits))
                {
                    setAxisUnits(horizUnits, vertUnits);
                }
                else
                {
                    setAxisUnits(horizUnits, vertUnits + AMath.DOT + horizUnits);
                }
                break;

            case TYPE_SWITCHED_Y_SKEW_TRANSFORM:
                setAxisLabels(vertLabel + AMath.PRIME, AMath.DELTA + horizLabel);
                if (vertUnits.equals(horizUnits))
                {
                    setAxisUnits(vertUnits, horizUnits);
                }
                else
                {
                    setAxisUnits(vertUnits + AMath.DOT + horizUnits, horizUnits);
                }
                break;

            case TYPE_SWITCHED_X_SKEW_TRANSFORM:
                setAxisLabels(AMath.DELTA + vertLabel, horizLabel + AMath.PRIME);
                if (horizUnits.equals(vertUnits))
                {
                    setAxisUnits(vertUnits, horizUnits);
                }
                else
                {
                    setAxisUnits(vertUnits, horizUnits + AMath.DOT + vertUnits);
                }
                break;

            default:
                setAxisLabels(horizLabel + AMath.PRIME, vertLabel + AMath.PRIME);
                if (horizUnits.equals(vertUnits))
                {
                    setAxisUnits(horizUnits, vertUnits);
                }
                else
                {
                    setAxisUnits(horizUnits + AMath.DOT + vertUnits, vertUnits + AMath.DOT + horizUnits);
                }
                break;
        }

        // Get the size of the scales.
        axisPts[0] = 0.;
        axisPts[1] = panelHeight;
        axisPts[2] = 0.;
        axisPts[3] = panelHeight;
        axisPts[4] = panelWidth;
        axisPts[5] = 0.;
        try
        {
            // Avoid using the following call because of a bug in
            // AffineTransform. Instead create the inverse matrix
            // explicitly as done below.
            // transform.inverseTransform(physicsPt,0,physicsPt,0,3);
            AffineTransform ixform = transform.createInverse();

            ixform.transform(axisPts, 0, axisPts, 0, 1);
            ixform.deltaTransform(axisPts, 2, axisPts, 2, 2);

            // Calculate the values for the vertical axis and the distance.
            double vdy = axisPts[3];
            double vdx = axisPts[2];
            double vdist = Math.sqrt(vdx * vdx + vdy * vdy);

            // Calculate the values for the horizontal axis and the distance.
            double hdy = axisPts[5];
            double hdx = axisPts[4];
            double hdist = Math.sqrt(hdx * hdx + hdy * hdy);

            // Initialize the endpoints of the axes.
            double vmin = 0.;
            double vmax = 0.;
            double hmin = 0.;
            double hmax = 0.;

            // Do what is necessary for the different types of transformations.
            switch (type)
            {
                case (TYPE_PARALLEL_TRANSFORM):
                {
                    double vsign = (vdy < 0.) ? 1. : -1.;

                    vmin = axisPts[1];
                    vmax = axisPts[1] + vsign * vdist;

                    double hsign = (hdx > 0.) ? 1. : -1.;

                    hmin = axisPts[0];
                    hmax = axisPts[0] + hsign * hdist;
                    break;
                }

                case (TYPE_SWITCHED_TRANSFORM):
                {
                    double hsign = (hdy > 0.) ? 1. : -1.;

                    hmin = axisPts[1];
                    hmax = axisPts[1] + hsign * hdist;

                    double vsign = (vdx < 0.) ? 1. : -1.;

                    vmin = axisPts[0];
                    vmax = axisPts[0] + vsign * vdist;
                    break;
                }

                case (TYPE_Y_SKEW_TRANSFORM):
                {
                    double vsign = (vdy > 0.) ? 1. : -1.;

                    vmax = -vsign * vdist / 2.;
                    vmin = -vmax;

                    hmin = 0.;
                    hmax = hdist;
                    break;
                }

                case (TYPE_X_SKEW_TRANSFORM):
                {
                    double hsign = (hdx > 0.) ? 1. : -1.;

                    hmax = hsign * hdist / 2.;
                    hmin = -hmax;

                    vmin = 0.;
                    vmax = vdist;
                    break;
                }

                case (TYPE_SWITCHED_Y_SKEW_TRANSFORM):
                {
                    double vsign = (vdx > 0.) ? 1. : -1.;

                    vmax = -vsign * vdist / 2.;
                    vmin = -vmax;

                    hmin = 0.;
                    hmax = hdist;
                    break;
                }

                case (TYPE_SWITCHED_X_SKEW_TRANSFORM):
                {
                    double hsign = (hdy > 0.) ? 1. : -1.;

                    hmax = hsign * hdist / 2.;
                    hmin = -hmax;

                    vmin = 0.;
                    vmax = vdist;
                    break;
                }

                default:
                {
                    vmin = 0.;
                    vmax = 0.;
                    hmin = 0.;
                    hmax = 0.;
                    break;
                }
            }

            // Actually set the limits.
            setLimits(hmin, hmax, vmin, vmax);
        }
        catch (NoninvertibleTransformException e)
        {
            setLimits(0., 0., 0., 0.);
        }
    }

    /**
     * This is a protected utility method which classifies the given transform
     * into seven categories: parallel, switched, x-skew, y-skew, switched
     * x-skew, switched y-skew, and general. The parallel category describes
     * transformations in which the transformed x and y axes are parallel or
     * antiparallel to the original x and y axes, respectively. The switched
     * category describes transformations in which the transformed x and y axes
     * are parallel or antiparallel to the original y and x axes, respectively.
     * That is, the x and y axes have been switched. The x-skew describes
     * transformations in which the transformed x-axis is parallel (or
     * antiparallel) to the original one while the transformed y-axis forms some
     * non-zero angle to the original one. The y-skew is similar; the switch
     * skews are just rotated (counter)clockwise by 90 degrees. The general
     * category encompasses all transforms not falling into one of the other
     * categories.
     */
    static protected int classifyTransform(AffineTransform xform)
    {
        // Set the default return type to a general matrix.
        int category = TYPE_GENERAL_TRANSFORM;

        // Get the four non-translation quantities from the transformation.
        double sx = xform.getScaleX();
        double sy = xform.getScaleY();
        double kx = xform.getShearX();
        double ky = xform.getShearY();

        // Check the type.
        if (kx == 0. && ky == 0.)
        {
            category = TYPE_PARALLEL_TRANSFORM;
        }
        else if (sx == 0. && sy == 0.)
        {
            category = TYPE_SWITCHED_TRANSFORM;
        }
        else if (kx == 0.)
        {
            category = TYPE_Y_SKEW_TRANSFORM;
        }
        else if (ky == 0.)
        {
            category = TYPE_X_SKEW_TRANSFORM;
        }
        else if (sx == 0.)
        {
            category = TYPE_SWITCHED_Y_SKEW_TRANSFORM;
        }
        else if (sy == 0.)
        {
            category = TYPE_SWITCHED_X_SKEW_TRANSFORM;
        }

        // Return the transformtion type.
        return category;
    }

    /**
     * A utility which makes an AffineTransform given three corner
     * points. The first point must be the upper, left-hand corner
     * point, the second, the upper, right-hand corner point, and the
     * third, the lower, right-hand corner point.
     *
     * @return AffineTransform which does the appropriate mapping */
    protected AffineTransform makeTransform(Point2D.Double[] corners)
    {
        double x0 = corners[0].x;
        double y0 = corners[0].y;
        double x1 = corners[1].x;
        double y1 = corners[1].y;
        double x2 = corners[2].x;
        double y2 = corners[2].y;

        double sx = 0.;
        double kx = 0.;
        double tx = 0.;
        double sy = 0.;
        double ky = 0.;
        double ty = 0.;

        double delta = (x2 * (y1 - y0) - x1 * (y2 - y0) + x0 * (y2 - y1));

        if (delta == 0)
        {
            // cannot successfully create an AffineTransform
            System.err.println("Fatal Error: AffineTransform creating failed.");
            System.err.println("Possible cause: wrong user corner values are set.");
            System.exit(1);
            return null;
        }
        else
        {
            delta = 1. / delta;

            double w = aWindow.getWidth();
            double h = aWindow.getHeight();

            sx = -(delta * w) * (y2 - y1);
            kx = (delta * w) * (x2 - x1);
            tx = -(x0 * sx + y0 * kx);

            ky = (delta * h) * (y1 - y0);
            sy = -(delta * h) * (x1 - x0);
            ty = -(x0 * ky + y0 * sy);

            return new AffineTransform(sx, ky, kx, sy, tx, ty);
        }

    }

    public void drawString(Graphics2D g, String str, double x, double y, int horizontal, int vertical, int maxSize)
    {
        drawString(g, str, x, y, horizontal, vertical, false, null, 0, false, null, maxSize);
    }

    private void drawString(Graphics2D g, String str, double x, double y, int horizontal, int vertical, boolean framed, Color frameColor, double frameWidth, boolean banner, Color bannerColor, int maxSize)
    {
        Font sf = g.getFont();
        int width = sf.getSize();
        int descent = 0;
        int height = sf.getSize();
        int adjustment = 0;

        // The metrics stuff did not work, so I made a work around and now
        // even ps-printing works (CT 27/7/2004)
        double xx = Math.min(x, maxSize - width / 2);
        Rectangle textSize = new Rectangle(0, descent - height, width, height);
        Point2D textUL = drawFrameAndBanner(xx, y, textSize, adjustment, framed, frameColor, frameWidth, banner, bannerColor, horizontal, vertical);

        if (g instanceof AAbstractGraphics2D) {
            // No antialiasing support needed for EPS output.
            g.drawString(str, (int) textUL.getX(), (int) textUL.getY());
        } else {
            // For pixel graphics, switch on antialiasing for text.
            g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            g.drawString(str, (int) textUL.getX(), (int) textUL.getY());
            g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
        }
    }

    /** Draws frame and banner around a string. The method calculates and returns
     *  the point to which the text curser should be set before drawing the string. */
    private Point2D drawFrameAndBanner(double x, double y, Rectangle textSize, int adjustment, boolean framed, Color frameColor, double frameWidth, boolean banner, Color bannerColor, int horizontal, int vertical)
    {
        double descent = textSize.y + textSize.height;
        x = getXalignment(x, textSize.width, horizontal);
        y = getYalignment(y, textSize.height, (int) descent, vertical);
        return new Point2D.Double(x, y);
    }

    private static double getYalignment(double y, int ascent, int descent, int alignment)
    {
        // vertical alignment
        switch (alignment)
        {
            case TEXT_TOP:
                y = y + ascent - descent;
                break;

            case TEXT_CENTER:
                y = y + ((ascent + descent) / 2) - descent;
                break;

            case TEXT_BOTTOM:
                y = y - descent;
                break;

            case TEXT_BASELINE:
            default:
                break;
        }
        return y;
    }

    private static double getXalignment(double x, int width, int alignment)
    {
        // horizontal alignment
        switch (alignment)
        {
            case TEXT_CENTER:
                x = x - (width / 2);
                break;

            case TEXT_RIGHT:
                x = x - width;
                break;

            case TEXT_LEFT:
            default:
                break;
        }
        return x;
    }
    
    /**
     * Encapsulates units and range for an axis.
     * Used for m->cm->mm->um autoscaling.
     */
    private static class ScaleUnits {
    	final String units;
    	final double scaleFactor;
    	
    	ScaleUnits(String units, double min, double max) {
            double maxMod = Math.max(Math.abs(max), Math.abs(min));
            if (!units.equals("(cm)")) {
            	this.units = units;
            	this.scaleFactor = 1.0;
            	return;
            }
            else {
                if (maxMod > 200.)
                {
                    this.scaleFactor= 0.01;
                    this.units = "(m) ";
                }
                else if (maxMod > 2.)
                {
                    this.scaleFactor = 1.0;
                    this.units = "(cm)";
                }
                else if (maxMod > .2)
                {
                    this.scaleFactor = 10.;
                    this.units = "(mm)";
                }
                else
                {
                    this.scaleFactor = 10000.;
                    this.units = "(" + AMath.MICRO + "m)";
                }
            }
    	}
    }

}
