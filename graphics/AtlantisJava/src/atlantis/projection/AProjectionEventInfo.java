package atlantis.projection;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.geom.Point2D;
import javax.swing.ImageIcon;
import javax.swing.JMenuItem;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.data.ADHelix;
import atlantis.event.AEvent;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.utils.AUtilities;

/** Class used to display the ATLAS Experiment logo, along with
 *  Event Number, Run Number, and Date/Time
 *
 *  @author Tom McLaughlan
 */
public class AProjectionEventInfo extends AProjectionInfo {

    private static long EventNumber;
    private static long RunNumber;
    private static String DateTime;
    private static String Source;
    private static Color textColor = Color.white,
            backgroundColor = Color.black;

    public String getName() {
        return "EventInfo";
    }

    public void paint(AWindow window, Graphics g) {

        AEvent event = eventManager.getCurrentEvent();

        if (event != null) {
            RunNumber = event.getRunNumber();
            EventNumber = event.getEventNumber();
            DateTime = event.getDateTime();
            Source = event.getSourceName();
        }

        // Begin drawing
        AGraphics ag = AGraphics.makeAGraphics(g);
        // Fill the background
        ag.setColor(backgroundColor);
        ag.fillRect(0, 0, window.getWidth(), window.getHeight());

        String iconPath = AGlobals.instance().getHomeDirectory() + "img" + System.getProperty("file.separator");

        ImageIcon i = AUtilities.getFileAsImageIcon(iconPath + "atlas_logo_shadow.png");


        // Determine height and width of current window, and width of ATLAS logo
        // These are then used to determine a scaling factor which is used in the
        // scale function.
        //
        int height = window.getSize().height;
        int width = window.getSize().width;
        int iwidth = i.getIconWidth();
        double factor = (double) width / (double) iwidth;

        // Call scale function to scale image to window size
        ImageIcon iscale = AUtilities.scale(i.getImage(), factor, parameterStore.get("Prefs", "AntiAlias").getStatus());
        // x, y calculated to position image exactly centred
        int x = (width / 2) - (iscale.getIconWidth() / 2);
        int y = (height / 2) - ((3 * iscale.getIconHeight()) / 4);

        // get screen dpi for resolution independent scaling
        // commented out 27/3 -- EJ
        //   without a screen you obviously do not have a screen resolution
        int dpi = 72; //Toolkit.getDefaultToolkit().getScreenResolution();

        // Set a scaling font size with window width
        int fontSize = (int) Math.round(3.0 * width / (double) dpi);

        Font f = new Font("SansSerif", Font.PLAIN, fontSize);

        ag.drawImage(iscale.getImage(), x, y);

        ag.setColor(textColor); // Set colour for text
        ag.updateColor(); // Update current colour for drawing

        String DateString = "", RunEventString = "";
        String DisclaimerString1 = "", DisclaimerString2 = "";
        if (event != null) {
            DateString = "Date: " + DateTime;
            RunEventString = "Run Number: " + RunNumber + ", Event Number: " + EventNumber;
            DisclaimerString1 = "Snapshot of a proton collision";
            DisclaimerString2 = "directly from the ATLAS experiment";
        } else {
            RunEventString = "No event data available";
        }

        FontMetrics fm = g.getFontMetrics(f);
        int DateWidth = fm.stringWidth(DateString);
        int RunEventWidth = fm.stringWidth(RunEventString);
        int Disclaimer1Width = fm.stringWidth(DisclaimerString1);
        int Disclaimer2Width = fm.stringWidth(DisclaimerString2);

        // This may need changing to be a bit more elegant
        // -- currently under investigation by Tom

        // Check if font is wider than the window and scale down til it fits.
        while ((RunEventWidth > width - 20) || (DateWidth > width - 20))
        {

            fontSize = fontSize - 1;
            f = new Font("SansSerif", Font.PLAIN, fontSize);
            fm = g.getFontMetrics(f);
            RunEventWidth = fm.stringWidth(RunEventString);
            DateWidth = fm.stringWidth(DateString);
            Disclaimer1Width = fm.stringWidth(DisclaimerString1);
            Disclaimer2Width = fm.stringWidth(DisclaimerString2);
        }

        ag.setFont(f); // set font as defined above

        // For the streams at P1, we include a disclaimer
        // Anything without .xml is considered a stream for now
        if (Source != null && !Source.endsWith(".xml")) {
            ag.drawString(DisclaimerString1,
                    (width / 2) - Disclaimer1Width / 2, 0.89 * height);
            ag.drawString(DisclaimerString2,
                    (width / 2) - Disclaimer2Width / 2, 0.95 * height);
        }

        if (event != null && !DateTime.equals("") && !DateTime.equals("n/a")) {
            // If event contains Date/Time data, draw Run Number, Event Number and Date/Time
            // and position text in centre of window
            ag.drawString(RunEventString,
                    (width / 2) - RunEventWidth / 2, y + (1.1 * iscale.getIconHeight()));

            ag.drawString(DateString,
                    (width / 2) - DateWidth / 2, y + (1.3 * iscale.getIconHeight()));
        } else {
            // Draw text (No Date/Time, for MC events, etc)
            ag.drawString(RunEventString,
                    (width / 2) - RunEventWidth / 2, y + (1.1 * iscale.getIconHeight()));

        }

    }

    public JMenuItem[] getPopupItems() {
        return null;
    }

    public String getXLabel() {
        return "";
    }

    public String getXUnits() {
        return "";
    }

    public String getYLabel() {
        return "";
    }

    public String getYUnits() {
        return "";
    }

    @Override
    public void actionPerformed(ActionEvent e) {
    	assert false : "Did not expect AProjectionEventInfo to be used as ActionListener";
    }

    public void setScales() {
    }

    public Point2D.Double getCenter() {
        return new Point2D.Double(0.0, 0.0);
    }

    public Point2D.Double[] calculateNoZoomCorners(Dimension wSize) {
        Point2D.Double[] corners = new Point2D.Double[3];

        corners[0] = new Point2D.Double(0., 2.);
        corners[1] = new Point2D.Double(100., 2.);
        corners[2] = new Point2D.Double(100., -2.);
        return corners;
    }

    public ACoord getUserPoint(ADHelix dH, double s) {
        return ACoord.NO_DATA;
    }
}//end of EventInfoProjection class
