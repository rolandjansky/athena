package atlantis.canvas;

import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;

import javax.swing.ImageIcon;
import javax.swing.JPanel;

import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.globals.AGlobals;
import atlantis.graphics.AGraphics;
import atlantis.parameters.APar;
import atlantis.utils.AUtilities;

/**
 * AOverlay should be used within a GlassPane in the ACanvas.
 * The overlay acts as a canvas on which images can be drawn over the existing display.
 * This is currently only used to draw the ATLAS logo and Run/Event info as a floating image.
 * Any drawing should be done with the paintComponent method, which is called as the Canvas is drawn.
 * @author Tom McLaughlan
 */
public class AOverlay extends JPanel {

	protected static APar parameterStore = APar.instance();
	
	public AOverlay() {
		setOpaque(false);
	}

	/**
	 * When the ACanvas GlassPane contains an Overlay instance, this is called 
	 * after the Canvas is drawn.
	 */
	@Override public void paintComponent(Graphics g) {
		super.paintComponent(g);
		if(parameterStore.get("Logo","ShowATLAS").getStatus())
			drawATLASLogo(g);
	}

	/**
	 * Draws a scaled version of the ATLAS PNG in the Overlay
	 * @param g The Graphics instance to draw to
	 */
	protected void drawATLASLogo(Graphics g) {

		int windowWidth = getWidth();
		int windowHeight = getHeight();

		int logoWidth = 0;
		int logoHeight = 0;

		double size = parameterStore.get("Logo", "LogoSize").getD();

		logoWidth = (int) (windowWidth*size);
		logoHeight = (int) (logoWidth*0.6);

		int logoX = 0;
		int logoY = 0;

		double hpos = parameterStore.get("Logo", "LogoHPos").getD();
		logoX = (int) ((hpos * (windowWidth-logoWidth)));

		double vpos = parameterStore.get("Logo", "LogoVPos").getD();

		int heightOffset = 0;
		if(parameterStore.get("Prefs", "CanvasTitle").getStatus()) {
			windowHeight = this.getParent().getHeight() - 40;
			heightOffset = 40;
		}
		logoY = (int) ((vpos * (windowHeight-logoHeight))) + heightOffset;
		
		
		long EventNumber = 0;
		long RunNumber = 0;
		String DateTime = "";
		Color textColor = Color.white,
		backgroundColor = Color.black;

		AEvent event = AEventManager.instance().getCurrentEvent();

		if (event != null) {
			RunNumber = event.getRunNumber();
			EventNumber = event.getEventNumber();
			DateTime = event.getDateTime();
		}

		// Begin drawing
		AGraphics ag = AGraphics.makeAGraphics(g);
		// Fill the background
		ag.setColor(backgroundColor);
		//ag.fillRect(logoX, logoY, logoWidth, logoHeight);

		String iconPath = AGlobals.instance().getHomeDirectory() + "img" + System.getProperty("file.separator");

		ImageIcon i = AUtilities.getFileAsImageIcon(iconPath + "atlas_logo_shadow.png");


		// Determine height and width of current window, and width of ATLAS logo
		// These are then used to determine a scaling factor which is used in the
		// scale function.
		//

		int iwidth = i.getIconWidth();
		double factor = (double) logoWidth / (double) iwidth;

		// Call scale function to scale image to window size
		ImageIcon iscale = AUtilities.scale(i.getImage(), factor, parameterStore.get("Prefs", "AntiAlias").getStatus());
		// x, y calculated to position image exactly centred
		int x = (logoWidth / 2) - (iscale.getIconWidth() / 2);
		int y = (logoHeight / 2) - ((3 * iscale.getIconHeight()) / 4);

		x += logoX;
		y += logoY;

		int dpi = 72;

		// Set a scaling font size with window width
		int fontSize = (int) Math.round(3.0 * logoWidth / (double) dpi) - 2;

		Font f = new Font("SansSerif", Font.PLAIN, fontSize);

		ag.drawImage(iscale.getImage(), x, y);

		ag.setColor(textColor); // Set colour for text
		ag.updateColor(); // Update current colour for drawing

		String DateString = "", RunEventString = "";
		if (event != null) {
			DateString = "Date: " + DateTime;
			RunEventString = "Run Number: " + RunNumber + ", Event Number: " + EventNumber;
		} else {
			RunEventString = "No event data available";
		}

		FontMetrics fm = g.getFontMetrics(f);
		int DateWidth = fm.stringWidth(DateString);
		int RunEventWidth = fm.stringWidth(RunEventString);

		// Check if font is wider than the window and scale down til it fits.
		while ((RunEventWidth > logoWidth - 20) || (DateWidth > logoWidth - 20))
		{

			fontSize = fontSize - 1;
			f = new Font("SansSerif", Font.PLAIN, fontSize);
			fm = g.getFontMetrics(f);
			RunEventWidth = fm.stringWidth(RunEventString);
			DateWidth = fm.stringWidth(DateString);
		}

		ag.setFont(f); // set font as defined above

		if (event != null) {
			drawStrokedString(ag, RunEventString, logoX + (logoWidth / 2) - RunEventWidth / 2, (int) (y + (1.1 * iscale.getIconHeight())), Color.WHITE, Color.BLACK);
			//ag.drawString(RunEventString,
			//		logoX + (logoWidth / 2) - RunEventWidth / 2, y + (1.1 * iscale.getIconHeight()));

			if(!DateTime.equals("") && !DateTime.equals("n/a")) {
				// If event contains Date/Time data, draw Run Number, Event Number and Date/Time
				// and position text in centre of window
				drawStrokedString(ag, DateString, logoX + (logoWidth / 2) - DateWidth / 2, (int) (y + (1.3 * iscale.getIconHeight())), Color.WHITE, Color.BLACK);

				//ag.drawString(DateString,
				//		logoX + (logoWidth / 2) - DateWidth / 2, y + (1.3 * iscale.getIconHeight()));
			}
		}
	}
	
	/**
	 * Draws text with an outline, crudely.
	 * @param ag The AGraphics instance to draw to
	 * @param str The String to draw
	 * @param x x position of the string
	 * @param y y position of the string
	 * @param fg Text foreground colour
	 * @param bg Text outline colour
	 */
	protected void drawStrokedString(AGraphics ag, String str, int x, int y, Color fg, Color bg) {
		ag.setColor(Color.BLACK);
		ag.updateColor();
		ag.drawString(str, x-1, y-1);
		ag.drawString(str, x+1, y+1);
		ag.drawString(str, x+1, y-1);
		ag.drawString(str, x-1, y+1);
		ag.setColor(fg);
		ag.updateColor();
		ag.drawString(str, x, y);
	}


}
