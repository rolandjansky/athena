package atlantis.data;

import java.util.Vector;
import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.event.WindowEvent;
import java.awt.event.WindowAdapter;

import javax.swing.JFrame;
import javax.swing.JPanel;

// JAS libraries used by Plot class below
import hep.aida.IAnalysisFactory;
import hep.aida.ITree;
import hep.aida.IPlotter;
import hep.aida.IDataPoint;
import hep.aida.IDataPointSet;
import hep.aida.IDataPointSetFactory;
import hep.aida.IPlotterStyle;
import hep.aida.ref.plotter.PlotterUtilities;

import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;


/**
 * Plotting pulse shapes and ADC counts for calorimeters
 *
 * @author Zdenek Maxa

 */
public class APulseShapePlot
{
	protected static Component guiComponent = AGlobals.instance().getGuiFrame();
    private static Vector<JFrame> frameVector = new Vector<JFrame>();

    private APulseShapePlot() {}


    /**
     * Plot real pulse shapes plots - ADC counts and calculated values of the
     * real pulse shape plot over it. Works for multiple channels (TILE) as
     *  well as for a single channel e.g. LAr
     *
     * @param adcCounts int[][]
     * @param realPulse double[][]
     * @param step double
     * @param subTitle String[]
     * @param title String
     */
    public static void plotRealPulseShapes(int[][] adcCounts, double[][] realPulse,
                                           double step, String[] subTitle, String title)
    {
        JFrame frame = new JFrame(title);
        frame.setTitle(title);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        AIcon.setIconImage(frame);
        frame.setLocation(guiComponent.getLocation());
        Plot p = new Plot();
        frame.setContentPane(p);
        p.plotRealPulseShapes(adcCounts, realPulse, step, subTitle);
        frame.pack();
        int windowWidth = 330 + ((adcCounts.length - 1) * 110);
        int windowHeight = 300;
        frame.setSize(windowWidth, windowHeight);
        frame.setVisible(true);

        frame.addWindowListener(new AllWindowsDisposer());
        frameVector.add(frame);

    } // plotRealPulseShapes() ----------------------------------------------

    public static void plotRawCellPulseShapes(int[][] adcCounts, double[][] rawPulse,
                                           double[][] cellPulse, double step,
                                           String[] subTitle, String title)
    {
        JFrame frame = new JFrame(title);
        frame.setTitle(title);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        AIcon.setIconImage(frame);
        frame.setLocation(guiComponent.getLocation());
        Plot p = new Plot();
        frame.setContentPane(p);
        p.plotRawCellPulseShapes(adcCounts, rawPulse, cellPulse, step, subTitle);
        frame.pack();
        int windowWidth = 440 + ((adcCounts.length - 1) * 140);
        int windowHeight = 400;
        frame.setSize(windowWidth, windowHeight);
        frame.setVisible(true);

        frame.addWindowListener(new AllWindowsDisposer());
        frameVector.add(frame);

    } // plotRealPulseShapes() ----------------------------------------------    

    /**
     * Plots only ADC counts, used as follows:
     * APulseShapePlot.plotADCCounts(adcCounts[index], title)
     * still used for instance when real pulse shapes plot data is not
     * available, e.g. LAr presampler (layer 0)

     * @param data int[][]
     * @param title String title of window
     * @param axis String[] title for the horizontal axis on each plot (if null uses "bunch crossing")
     */
    public static void plotADCCounts(int[][] data, String title, String[] axis)
    {
        String[] labels = new String[data.length];
        String[] colors = new String[data.length];
        for(int i=0; i<data.length; i++)
        {
            labels[i]="ADC counts";
            colors[i]="blue";
        }
        String[] newAxis;
        if(axis!=null)
        {
            newAxis=axis;
        }
        else
        {
            newAxis= new String[data.length];
            for(int i=0; i<data.length; i++)
            {
                newAxis[i] = "bunch crossing";
            }
        }
        plotADCCounts(data, title, newAxis, labels, colors);
    }



    /**
     * Plots only ADC counts, used as follows:
     * APulseShapePlot.plotADCCounts(adcCounts[index], title)
     * still used for instance when real pulse shapes plot data is not
     * available, e.g. LAr presampler (layer 0)

     * @param data int[][]
     * @param title String title of window
     * @param axis String[] titles for the horizontal axis on each plot
     * @param labels String[] titles for each plot
     * @param colors String[] color of datapoints for each plot 
     */
    public static void plotADCCounts(int[][] data, String title, String[] axis, String[] labels, String[] colors)
    {
        JFrame frame = new JFrame(title);
        AIcon.setIconImage(frame);
        frame.setLocation(guiComponent.getLocation());
        Plot p = new Plot();
        frame.setContentPane(p);
        p.plotADCCounts(data, axis, labels, colors);
        frame.pack();
        int windowWidth = 330+ ((data.length - 1) * 110);
        int windowHeight = 300;
        frame.setSize(windowWidth, windowHeight);
        frame.setVisible(true);

        frame.addWindowListener(new AllWindowsDisposer());
        frameVector.add(frame);

    } // plotADCCounts() --------------------------------------------------



    static class AllWindowsDisposer extends WindowAdapter
    {
        public void windowClosing(WindowEvent e)
        {
            for(int i = 0; i < frameVector.size(); i++)
            {
                JFrame cf = frameVector.get(i);
                cf.dispose();
            }
            frameVector.removeAllElements();
        }

    } // class AllWindowsDisposer ===========================================


} // class ACaloPulseShapePlot ==============================================



/**
 *
 * <p>Title: Plot</p>
 *
 * <p>Description: Plotting class using JAS library </p>
 *
 */
class Plot extends JPanel
{

    public Plot()
    {
        super(new BorderLayout());

    } // Plot() -------------------------------------------------------------



    protected void plotRealPulseShapes(int[][] adcCounts, double[][] realPulse,
                                    double step, String[] subTitle)
    {

        IAnalysisFactory af = IAnalysisFactory.create();
        ITree tree = af.createTreeFactory().create();
        IDataPointSetFactory dpsf = af.createDataPointSetFactory(tree);

        // Create a two dimensional IDataPointSet
        IDataPointSet[] dataPointSet = new IDataPointSet[adcCounts.length];
        for(int i = 0; i < adcCounts.length; i++)
        {
            String myTitle = (subTitle != null) ? subTitle[i] : "Real pulse shape";
            
            double[] x = new double[adcCounts[i].length];
            double[] y = new double[adcCounts[i].length];
            double[] e = new double[adcCounts[i].length];

            for(int ii = 0; ii < adcCounts[i].length; ii++)
            {
                x[ii] = ii + 1.0; // counting from 1 (to 9 for TILE)
                y[ii] = (double) adcCounts[i][ii];
                e[ii] = 0.0; // arrays with errors
            }

            // first parameter must be path - what for here?
            dataPointSet[i] = dpsf.createXY(myTitle, x, y, e, e);
            dataPointSet[i].setTitle(myTitle);
        } // over number of channels

        // Create a two dimensional IDataPointSet
        IDataPointSet[] dataPointSetReal = new IDataPointSet[realPulse.length];
        for(int i = 0; i < realPulse.length; i++)
        {
        	String myTitle = (subTitle != null) ? subTitle[i] : "Real pulse shape";

            double[] x = new double[realPulse[i].length];
            double[] y = new double[realPulse[i].length];
            double[] e = new double[realPulse[i].length];

            double d = 1.0;
            for(int ii = 0; ii < realPulse[i].length; ii++)
            {
                x[ii] = d;
                y[ii] = realPulse[i][ii];
                e[ii] = 0.0; // arrays with errors
                d += step;
            }

            // first parameter must be path - what for here?
            dataPointSetReal[i] = dpsf.createXY(myTitle, x, y, e, e);
            dataPointSetReal[i].setTitle(myTitle);

        } // over number of channels


        IPlotter plotter = af.createPlotterFactory().create("");

        plotter.createRegions(adcCounts.length, 1); // regions 1x1, 1x2, ...

        // set common style parameters for both regions
        IPlotterStyle style = af.createPlotterFactory().createPlotterStyle();
        style.xAxisStyle().setLabel("bunch crossing");
        style.yAxisStyle().setLabel("ADC counts");
        style.titleStyle().textStyle().setColor("black");
        style.titleStyle().textStyle().setBold(true);
        style.xAxisStyle().labelStyle().setColor("black");
        style.yAxisStyle().labelStyle().setColor("black");

        // set no legend at plots
        style.statisticsBoxStyle().setVisible(false); // statistical legend
        style.legendBoxStyle().setVisible(false); // legend for function fit

        for(int i = 0; i < adcCounts.length; i++)
        {
            // set style parameters different for regions
            style.dataStyle().markerStyle().setParameter("size", "2");
            style.dataStyle().markerStyle().setShape("box"); // "square"
            plotter.region(i).style().dataStyle().markerStyle().setColor("red");
            plotter.region(i).plot(dataPointSetReal[i], style);

            style.dataStyle().markerStyle().setParameter("size", "7");
            style.dataStyle().markerStyle().setShape("dot"); // "circle"
            plotter.region(i).style().dataStyle().markerStyle().setColor("blue");
            plotter.region(i).plot(dataPointSet[i], style);
        }

        // now embed the plotter
        add(PlotterUtilities.componentForPlotter(plotter), BorderLayout.CENTER);

    } // plotRealPulseShapes() ----------------------------------------------




    /**
     * Method creates multiple plots of ADC counts only.
     * multiple channels, no fit, no pulse shapes.
     *  or plots of ADC Counts pulse shapes.
     *
     * @param data int[][]
     * @param labels String[] titles for each plot
     */
    public void plotADCCounts(int[][] data, String[] axis, String[] labels, String[] colors)
    {
        IAnalysisFactory af = IAnalysisFactory.create();
        ITree tree = af.createTreeFactory().create();
        IDataPointSetFactory dpsf = af.createDataPointSetFactory(tree);

        // Create a IDataPointSet (one dimension)
        IDataPointSet[] dataPointSet = new IDataPointSet[data.length];
        for(int j = 0; j < data.length; j++)
        {

            dataPointSet[j] = dpsf.create("plot", labels[j], 2);

            for(int i = 0; i < data[j].length; i++)
            {
                dataPointSet[j].addPoint();
                IDataPoint dp = dataPointSet[j].point(i);

                dp.coordinate(0).setValue(i);
                dp.coordinate(1).setValue(data[j][i]);
            }
        }


        IPlotter plotter = af.createPlotterFactory().create("");

        plotter.createRegions(data.length, 1); // regions 1x1, 1x2, ...

        IPlotterStyle style = af.createPlotterFactory().createPlotterStyle();
        style.yAxisStyle().setLabel("ADC counts");
        style.dataStyle().markerStyle().setParameter("size", "7");
        style.dataStyle().markerStyle().setShape("dot");
        style.titleStyle().textStyle().setColor("black");
        style.titleStyle().textStyle().setBold(true);
        style.xAxisStyle().labelStyle().setColor("black");
        style.yAxisStyle().labelStyle().setColor("black");

        // set no legend at plots
        style.statisticsBoxStyle().setVisible(false); // statistical legend
        style.legendBoxStyle().setVisible(false); // legend for function fit

        for(int j = 0; j < data.length; j++)
        {
            plotter.region(j).style().xAxisStyle().setLabel(axis[j]);
            plotter.region(j).style().dataStyle().markerStyle().setColor(colors[j]);
            plotter.region(j).style().dataStyle().outlineStyle().setColor(colors[j]);

            // plot the data - points ('style' parameter is optional for plotter)
            plotter.region(j).plot(dataPointSet[j], style);
        }
        // now embed the plotter
        add(PlotterUtilities.componentForPlotter(plotter), BorderLayout.CENTER);

    } // plotADCCounts() ------------ ---------------------------------------

    
    protected void plotRawCellPulseShapes(int[][] adcCounts, double[][] rawPulse,
                                    double[][] cellPulse,
                                    double step, String[] subTitle)
    {

        IAnalysisFactory af = IAnalysisFactory.create();
        ITree tree = af.createTreeFactory().create();
        IDataPointSetFactory dpsf = af.createDataPointSetFactory(tree);

        // Create a two dimensional IDataPointSet
        IDataPointSet[] dataPointSet = new IDataPointSet[adcCounts.length];
        for(int i = 0; i < adcCounts.length; i++)
        {
            String myTitle = "ADC counts";
            
            double[] x = new double[adcCounts[i].length];
            double[] y = new double[adcCounts[i].length];
            double[] e = new double[adcCounts[i].length];

            for(int ii = 0; ii < adcCounts[i].length; ii++)
            {
                x[ii] = ii + 1.0; // counting from 1 (to 9 for TILE)
                y[ii] = (double) adcCounts[i][ii];
                e[ii] = 0.0; // arrays with errors
            }

            // first parameter must be path - what for here?
            dataPointSet[i] = dpsf.createXY(myTitle, x, y, e, e);
            dataPointSet[i].setTitle(myTitle);
        } // over number of channels

        // Create a two dimensional IDataPointSet
        IDataPointSet[] dataPointSetCell = new IDataPointSet[cellPulse.length];
        for(int i = 0; i < cellPulse.length; i++)
        {
        	String myTitle = "Cell time";

            double[] x = new double[cellPulse[i].length];
            double[] y = new double[cellPulse[i].length];
            double[] e = new double[cellPulse[i].length];

            double d = 1.0;
            for(int ii = 0; ii < cellPulse[i].length; ii++)
            {
                x[ii] = d;
                y[ii] = cellPulse[i][ii];
                e[ii] = 0.0; // arrays with errors
                d += step;
            }

            // first parameter must be path - what for here?
            dataPointSetCell[i] = dpsf.createXY(myTitle, x, y, e, e);
            dataPointSetCell[i].setTitle(myTitle);

        } // over number of channels

        
        // Create a two dimensional IDataPointSet
        IDataPointSet[] dataPointSetRaw = new IDataPointSet[rawPulse.length];
        for(int i = 0; i < rawPulse.length; i++)
        {
        	String myTitle = "Raw time";

            double[] x = new double[rawPulse[i].length];
            double[] y = new double[rawPulse[i].length];
            double[] e = new double[rawPulse[i].length];

            double d = 1.0;
            for(int ii = 0; ii < rawPulse[i].length; ii++)
            {
                x[ii] = d;
                y[ii] = rawPulse[i][ii];
                e[ii] = 0.0; // arrays with errors
                d += step;
            }

            // first parameter must be path - what for here?
            dataPointSetRaw[i] = dpsf.createXY(myTitle, x, y, e, e);
            dataPointSetRaw[i].setTitle(myTitle);

        } // over number of channels


        IPlotter plotter = af.createPlotterFactory().create("");

        plotter.createRegions(adcCounts.length, 1); // regions 1x1, 1x2, ...

        // set common style parameters for both regions
        IPlotterStyle style = af.createPlotterFactory().createPlotterStyle();
        style.xAxisStyle().setLabel("bunch crossing");
        style.yAxisStyle().setLabel("ADC counts");
        style.titleStyle().textStyle().setColor("black");
        style.titleStyle().textStyle().setBold(true);
        style.xAxisStyle().labelStyle().setColor("black");
        style.yAxisStyle().labelStyle().setColor("black");
        
        style.statisticsBoxStyle().setVisible(false); // statistical legend
        style.legendBoxStyle().setVisible(true); // legend for function fit

        for(int i = 0; i < adcCounts.length; i++)
        {
            // set style parameters different for regions
            style.dataStyle().markerStyle().setParameter("size", "4");
            style.dataStyle().markerStyle().setShape("box"); // "square"
            style.dataStyle().showInLegendBox(true);
            plotter.region(i).style().dataStyle().markerStyle().setColor("red");
            plotter.region(i).plot(dataPointSetRaw[i], style);
            
            style.dataStyle().markerStyle().setParameter("size", "2");
            style.dataStyle().markerStyle().setShape("box"); // "square"
            style.dataStyle().showInLegendBox(true);
            plotter.region(i).style().dataStyle().markerStyle().setColor("green");
            plotter.region(i).plot(dataPointSetCell[i], style);
            
            style.dataStyle().markerStyle().setParameter("size", "7");
            style.dataStyle().markerStyle().setShape("dot"); // "circle"
            style.dataStyle().showInLegendBox(false);
            plotter.region(i).style().dataStyle().markerStyle().setColor("blue");
            plotter.region(i).plot(dataPointSet[i], style);
            
            //plotter.region(i).style().legendBoxStyle().boxStyle().borderStyle().setVisible(false);
            
            plotter.region(i).setTitle((subTitle != null) ? subTitle[i] : "Real pulse shape");
        }

        // now embed the plotter
        add(PlotterUtilities.componentForPlotter(plotter), BorderLayout.CENTER);

    } // plotRealPulseShapes() ----------------------------------------------

    
    
} // class Plot =============================================================



/* following lines for hints only (futher JAS plotting styles settings)

     IPlotterStyle style = plotterFactory.createPlotterStyle();
     style.xAxisStyle().tickLabelStyle().setFontSize(14);
     style.xAxisStyle().tickLabelStyle().setBold(true);
     style.xAxisStyle().tickLabelStyle().setColor("blue");
     style.xAxisStyle().labelStyle().setFontSize(24);
     style.xAxisStyle().labelStyle().setItalic(true);
     style.titleStyle().textStyle().setFontSize(30);
     style.dataStyle().markerStyle().setParameter("size","12");
     style.dataStyle().markerStyle().setParameter("shape","3");
     style.dataStyle().markerStyle().setParameter("color","blue");
     plotter.region(0).plot(h1);
     plotter.region(0).plot(h2, style);
     plotter.show();
*/
