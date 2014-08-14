package atlantis.canvas;

import atlantis.event.AEvent;
import atlantis.event.ANewEventListener;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.geom.Point2D;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Stack;
import java.util.Vector;
import java.util.Set;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;

import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import atlantis.globals.AGlobals;
import atlantis.graphics.ACursorFactory;
import atlantis.graphics.AIcon;
import atlantis.graphics.layout.AGridLayout;
import atlantis.parameters.APar;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.utils.xml.AXMLUtils;
import atlantis.gui.AClosingConfirmationDialog;
import atlantis.gui.AInteractionToolBar;
import atlantis.gui.APreferencesControl;
import atlantis.interactions.AZMRInteraction;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionEventInfo;
import atlantis.utils.ALogger;
import java.awt.Graphics;
import java.awt.event.WindowFocusListener;
import java.util.Iterator;
import javax.swing.JMenuBar;
import javax.swing.JPanel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * ACanvas is a singleton class that acts as a sort of WindowManager It knows the currentLayout (of the
 * windows), their history and the currentWindow. It also contains the title (Menu Bar).
 * Window changes are given via two letter commands e.g. "W5" inserted into the
 * CommandProcessor (static). The command processor informs ACanvas.
 * Each window also has its own layer that is stored in a JLayeredPane object.
 */
public class ACanvas extends JPanel implements ANewEventListener {
    
    //Singleton instance
    private static ACanvas instance;
    
    private final static ALogger logger = ALogger.getLogger(ACanvas.class);
    
    /** The fraction of the screen width used by default. */
    public final double SCREEN_USAGE = 0.73;
    /** Temporary flag to set the window corners from config file. */
    protected boolean customisedCorner = false;
    
    //the layeredPane
    private JLayeredPane layeredPane;
    //from some reason we count or layers from 1000 on?
    private final int baseLayer = 1000;
    
    //A list of layout names and layouts
    private Hashtable<String, ALayout> layouts = new Hashtable<String, ALayout>();
    //The current canvas layout
    private ALayout currentLayout;
    //The name of the startup layout
    private String startupLayoutName;
    //Everyone that should get informed if the layout is changed
    private Vector<ALayoutChangeListener> layoutChangeListeners = new Vector<ALayoutChangeListener>();
    
    //Save a history of the windows layouts
    // - this is used for "Zoom/Unzoom colorimters" in VPlot
    private Stack<ALayout> windowLayoutHistory = new Stack<ALayout>();  //History of ALayout objects
    private Stack<String> windowNameHistory = new Stack<String>();    //History of the selected window
    private Stack<Vector<String>> windowsActive = new Stack<Vector<String>>();        //History of active (i.e. visible windows)
    
    private Hashtable<String,AWindow> windowsList;   // A list of all window <name,object> pairs
    private AWindow currentWindow;   // The currently selected window
    private AWindow paintingWindow;  // The window we are currently painting in
    //Everyone that should get informed about changes of the active window
    private Vector<ChangeListener> windowChangeListeners = new Vector<ChangeListener>();
    //Everyone that should get informed about window freeze/unfreeze changes
    private Vector<ChangeListener> lockChangeListeners = new Vector<ChangeListener>();
    
    //The title bar showing Event/Run numbers etc.
    private ATitleMenuBar title;
    
    //If constructed from a config file, this stores the aspect ratio of the
    //canvas layout which was saved into the config file, this is then used
    //later to correct if the current screen is different
    private double oldaspect = 1.0;

    private JFrame parentWindow = null;
    
    private APar parameterStore;
    
    /**
     * Get the singleton instance.
     * @return
     */
    public static synchronized ACanvas getCanvas() {
    	if (instance==null) throw new Error("Attempted to access ACanvas instance before initialization.");
        return instance;
    }
    
    /**
     * Create and return the singleton instance.
     * @param parameters
     */
    public static synchronized ACanvas construct(Node node) {
    	instance = new ACanvas(node);
    	return instance;
    }
    
    /**
     * Constructs the singleton instance by reading the config file.
     */
    private ACanvas(Node node) {

    	if (!AGlobals.isAtlantisHeadless()) {
            // Create parent window
            parentWindow = new JFrame("Atlantis Canvas");
            parentWindow.add(this);
        }
            
        //Create the title bar
        title = new ATitleMenuBar();
        
        if (!AGlobals.isAtlantisHeadless()) {
            parentWindow.setJMenuBar(title);
        }

        //instanciate instance pointer for singleton class
        instance = this;
        
        if (!AGlobals.isAtlantisHeadless()) {
            // AClosingConfirmationDialog class handles closing Atlantis
            parentWindow.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
            parentWindow.addWindowListener(new AClosingConfirmationDialog(parentWindow));
        
            //Set the window icon
            AIcon.setIconImage(parentWindow);
        }
        
        // building the Layered Pane
        layeredPane = new JLayeredPane();
        
        // reading all the content of Canvas configuration node
        windowsList = new Hashtable<String,AWindow>();
        // Allow null Node to generate partially initialized instance for unit tests of
        // other classes that depend on ACanvas.
        if (node!=null) readConfig(node);
        
    } // ACanvas() ----------------------------------------------------------

    /**
     * For testing only: create instance without initializing.
     * @return
     */
    public static synchronized ACanvas constructDummyForTesting() {
    	instance = new ACanvas(null);
    	return instance;
    }
    
    void addParentWindowFocusListener(WindowFocusListener wfl) {
        if (parentWindow != null) {
            parentWindow.addWindowFocusListener(wfl);
        }
    }
    
    public void setCanvasVisible(boolean v) {
        if (parentWindow != null) {
            parentWindow.setVisible(v);
        } else {
            setVisible(v);
        }
    }
    
    public Rectangle getScreenSize() {
        if (parentWindow != null) {
            return parentWindow.getGraphicsConfiguration().getBounds();
        } else {
            // If we're headless then make up a size!
            return new Rectangle(1024, 768);
        }
    }

    public Rectangle getStartupCanvasSize() {
        if (parentWindow == null) {
            return null;
        }
        Rectangle canvpos = parentWindow.getBounds();
        Dimension canvlayoutdim = parentWindow.getPreferredSize();
        Rectangle canvbound = new Rectangle(canvpos.x, canvpos.y,
                canvlayoutdim.width, canvlayoutdim.height);
        
        return canvbound;
    }
    
    public JMenuBar getTitleBar() {
        if (parentWindow == null) {
            return null;
        }
        return parentWindow.getJMenuBar();
    }
    
    public void bringToFront() {
        if (parentWindow != null) {
            parentWindow.toFront();
        }
    }
    
    /*public JFrame getParentWindow() {
        return parentWindow;
    }*/
    
    private void readConfig(Node node){
        
        //Get a list of subnodes
        NodeList children = node.getChildNodes();
        
        //Loop over all subnodes
        for (int i = 0; i < children.getLength(); i++) {
            Node child = children.item(i);
            //Check wether subnode is a Window or a Layout
            if (child.getNodeType() == Node.ELEMENT_NODE) {
                String nodeName = child.getNodeName();
                //Add a Window or a layout
                if (nodeName.equals("UsedWindow")) readWindow(child);
                else if (nodeName.equals("Layout")) readLayout(child);
            }
        }
        
        // reading all the attributes of Canvas node
        NamedNodeMap attributes = node.getAttributes();
        // The startupLayout:
        startupLayoutName = attributes.getNamedItem("startupLayout").getNodeValue().trim();
        
        // Aspect ratio stuff:
        oldaspect = Double.NaN;
        Node n = attributes.getNamedItem("aspectRatio");
        
        if (n != null) {
            oldaspect = Double.parseDouble(
                    attributes.getNamedItem("aspectRatio").getNodeValue().trim());
        }
        
        logger.debug("Read config file with aspectRatio: " + oldaspect);
    }
    
    //Read window configuration
    private void readWindow(Node node) {
        AWindow w = AWindow.createFromXMLConfigNode(node, getWindowsCount());
        
        if (!windowsList.containsKey(w.getName())) {
            windowsList.put(w.getName(), w);
        } else
            throw new Error("Redefinition of window: " + w.getName());
    }
    
    //Read layout configuration
    private void readLayout(Node node) {
        ALayout layout = new ALayout(node);
        
        if (!layouts.containsKey(layout.getName()))
            layouts.put(layout.getName(), layout);
        else
            throw new Error("Redefinition of layout: " + layout.getName());
    }
    
    //Read window corners
    public void readCorners(Node corners) {
        if (corners == null)
            return;
        //Set flag to set default corners
        customisedCorner = true;
        //Get list of corners
        NodeList children = corners.getChildNodes();
        //Loop over corners
        for (int i = 0; i < children.getLength(); i++) {
            Node corner = children.item(i);
            if (corner.getNodeType() == Node.ELEMENT_NODE) {
                //Get corner attributes
                NamedNodeMap attributes = corner.getAttributes();
                String windowName = AXMLUtils.tryAttribut(attributes, "windowName");
                double x0 = Double.parseDouble(AXMLUtils.tryAttribut(attributes, "x0"));
                double y0 = Double.parseDouble(AXMLUtils.tryAttribut(attributes, "y0"));
                double x1 = Double.parseDouble(AXMLUtils.tryAttribut(attributes, "x1"));
                double y1 = Double.parseDouble(AXMLUtils.tryAttribut(attributes, "y1"));
                double x2 = Double.parseDouble(AXMLUtils.tryAttribut(attributes, "x2"));
                double y2 = Double.parseDouble(AXMLUtils.tryAttribut(attributes, "y2"));
                //Create an array with new window corners
                Point2D.Double[] c = new Point2D.Double[] { new Point2D.Double(x0, y0), new Point2D.Double(x1, y1), new Point2D.Double(x2, y2) };
                //Set the corners for this window
                
                getWindow(windowName).setUserCorners(c);
            }
        }
        customisedCorner = false;
    }
    
    
    //Finalize the canvas construction
    public void finalizeConstruction() {
    	parameterStore = APar.instance();

        if (!parameterStore.get("Prefs", "CanvasTitle").getStatus()) {
            title.setVisible(false);
        }

        //Bring the startup layout on screen
        setCurrentLayout(startupLayoutName);
    }
    
    public void addOverlay() {
    	
    	AOverlay overlay = new AOverlay();
    	if(parentWindow != null)
    		parentWindow.setGlassPane(overlay);
    	overlay.setVisible(true);
    }
    
    public void correctAspectRatios() {
        
        // If we didn't have a saved aspect ratio or didn't know it when
        // the config was written
        if (Double.isNaN(oldaspect)) {
            return;
        }

        // Only correct aspect ratio for the full screen layout, the other
        // layouts have fixed aspect ratios and should always be okay
        if (!currentLayout.getName().equals("FULL SCREEN")) {
            return;
        }
        
        // If aspect ratios are the same, we can stop
        if (java.lang.Math.abs(getAspectRatio() - oldaspect) < 1e-2) {
            logger.debug("Aspect ratio of config file same as this display, no correction required");
            return;
        }
        
        logger.info("Aspect ratio of config file (" + oldaspect + ") does not match this display (" + getAspectRatio() + ") attempting to correct");
        
        // Else, iterate over all windows
        Iterator<AWindow> it = windowsList.values().iterator();
        while(it.hasNext()) {
            AWindow w = (AWindow)(it.next());
            AProjection p = w.getProjection();
            if (p instanceof AProjection2D) {
                ((AProjection2D)p).aspectRatioChange(w, oldaspect, getAspectRatio());
            }
        }
    }

    /**
     * Mimics the behavior of a normal canvas paint, but with the canvas scaled to a predefined size.
     * @param g Graphics object to draw on
     * @param imageWidth width of the image
     * @param imageHeight height of the image
     * @param type extension of image
     */
    public void drawScaledCanvas(Graphics g, int imageWidth, int imageHeight, String type)
    {
        //hide red ZMR center dot
        AZMRInteraction.setPaintCenterDot(false);

        //anti-alias pictures by default
        boolean aastatus = APreferencesControl.getAliasMenuItem();
        if(!aastatus)
        	APreferencesControl.setAliasMenuItem(true);
        
        // Calculate the scale factors.
       double factorX = ((double)imageWidth) / (double)getWidth();
       double factorY = ((double)imageHeight) / (double)getHeight();

       // Add the title bar, if required.
       if (title != null && parameterStore.get("Prefs", "CanvasTitle").getStatus()) {

         //Get old dimensions          
         int titleWidth = title.getWidth();
         int titleHeight = title.getHeight();
         
         if (AGlobals.isAtlantisHeadless()) {
             Dimension d = title.getLayout().preferredLayoutSize(title);
             titleWidth = (int)d.getWidth();
             titleHeight = (int)d.getHeight();
         }
         
         //Scale to image width (height is constant) and draw
         title.setSize(imageWidth, titleHeight);
         title.paint(g);
         //restore old
         title.setSize(titleWidth, titleHeight);
         g.translate(0, titleHeight);
         //Re-adjust y-scale for title height
         factorY = ((double)(imageHeight-titleHeight))/(double)getHeight();
       }

       // Loop over all windows.
       String[] windows = getCurrentLayout().getWindowNames();
       for (int i=0; i<windows.length; i++)
       {
           AWindow window = getWindow(windows[i]);
           int x = window.getX();
           int y = window.getY();
           int width = window.getWidth();
           int height = window.getHeight();
           // If the window is visible resize it temporarily and then draw it.
           if (isReallyOnScreen(window)){

               g.translate((int)(factorX*x), (int)(factorY*y));
               
               // Perhaps I should override window.setSize to automatically validate
               // This is because AWindow is now a container with a view inside
               // - Adam
               window.setSize((int)(factorX*width), (int)(factorY*height));
               window.validate();
               
               // Needed in headless mode because normally components don't
               // do layout until they're really drawn
               if (AGlobals.isAtlantisHeadless()) {
                   window.getLayout().layoutContainer(window);  
                   window.invalidateQuietly();
               }
               
               //Now print the actual windows
               window.print(g);

               // Show ATLAS logo only on the top left window if there's 
               // enough room and if the checkbox is selected
               if(x == 0 && y == 0 && (imageWidth > 300 || imageHeight > 250) &&
                  APreferencesControl.getAtlasLogoMenuItem()){
                 g.drawImage(ACursorFactory.getInstance().getAtlasLogo(),32,4,null);
               }
                //reset old dimensions
                window.setSize(width, height);
                window.validate();
                g.translate(-(int)(factorX*x), -(int)(factorY*y));
            }
        }

       // Overlay does not work in XMLRPC mode (no parentWindow)
       if (this.parentWindow != null) {
           //get the overlay, resize to our desired canvas size, draw it, then set it back to normal
            AOverlay overlay = (AOverlay) this.parentWindow.getGlassPane();
            int width = overlay.getWidth();
            int height = overlay.getHeight();
            overlay.setSize(imageWidth, imageHeight);
            overlay.paintComponent(g);
            overlay.setSize(width, height);
       }

        //do reset
        AZMRInteraction.setPaintCenterDot(true);
        if(!aastatus)
        	APreferencesControl.setAliasMenuItem(aastatus);
    }


    //Restore canvas defaults
    public void restoreDefaults() {
        //Current layout should never be null
        if (currentLayout != null) {
            if (!currentLayout.getName().equals(startupLayoutName)) {
                setCurrentLayout(startupLayoutName);
            }
            String[] frontWindows = currentLayout.getStartupSequence();
            String[] windows = this.getKnownWindowNames();
            for (String window : windows) windowsList.get(window).restoreDefaults();
            for (String window : frontWindows) this.moveToFrontWindow(window);
            setCurrentWindow(currentLayout.getStartupWindow());
        } else
            throw new Error("Current layout is not defined");
        
        // If an aspect ratio correction took place when we loaded this config
        // make sure it is reapplied on a reset - AD
        correctAspectRatios();
    }
    
    /*
     * Return a string containing the list of currently visible windows
     * that is then written to the config file by AConfigWriter
     */
    public String getStartupString() {
        StringBuilder s = new StringBuilder();
        //Loop over all windows in all layers
        int maxLayer = baseLayer + currentLayout.getWindowNames().length;
        for (int i = baseLayer; i <= maxLayer; i++) {
            Component[] c = layeredPane.getComponentsInLayer(i);
            for (int j = 0; j < c.length; j++)
                if (c[j] instanceof AWindow)
                    if (isReallyOnScreen((AWindow) c[j]))
                        s.append(((AWindow) c[j]).getName());
        }
        return s.toString();
    }
    
    /*
     * Bring this window to the front, i.e. make it the topmost window
     */
    public void moveToFrontWindow(String windowName) {
        //Make sure the window exists
        if (windowsList.containsKey(windowName)) {
            //Get the window
            AWindow w = windowsList.get(windowName);
            //Get the current layer of the window
            int layer = JLayeredPane.getLayer(w);
            //Get the other components int the same layer
            Component[] cc = layeredPane.getComponentsInLayer(layer);
            //Check for the current uppermost window
            int maxLayer = baseLayer + currentLayout.getWindowNames().length;
            
            //Now loop over all layers above the one the object is in
            for (int i = layer + 1; i <= maxLayer; i++) {
                //Get all components from this layers
                Component[] c = layeredPane.getComponentsInLayer(i);
                //Move them down one layer
                for (int j = 0; j < c.length; j++)
                    layeredPane.setLayer(c[j], i - 1);
            }
            //Finaly move all component from the current layer to the top
            for (int j = 0; j < cc.length; j++)
                layeredPane.setLayer(cc[j], maxLayer);
            //validate and repaint
            this.validate();
            if (!w.isVisible()) {
                w.setVisible(true);
            }
            hideNonVisible();
            w.repaintFromScratch();
        } else
            throw new Error("Cannot find window: " + windowName);
    }
    
    /*
     * Return window by name
     */
    public AWindow getWindow(String windowName) {
        //Search for the window in the list an return it
        if (windowsList.containsKey(windowName))
            return (AWindow) windowsList.get(windowName);
        else
            throw new Error("Cannot find window: " + windowName);
    }
    
    /*
     * Repaint all but the current window
     * Used to update SynchroCursor in all windows.
     */
    public void repaintOthers(Component current) {
        //Fill all windows in an enumeration object
        Enumeration<AWindow> myenum = windowsList.elements();
        
        //Loop over windows
        while (myenum.hasMoreElements()) {
            AWindow window = (AWindow) myenum.nextElement();
            //repaint those that are visible
            if ((window != current) && this.isReallyOnScreen(window))
                window.repaint();
        }
    }
    
    private void hideNonVisible() {
    	for (AWindow window : windowsList.values()) {
            if (this.isValidWindowName(window.getName())) {
                if (!this.isReallyOnScreen(window)) {
                    if (window.isVisible()) {
                        window.setVisible(false);
                    }
                }
            }
    	}
    }
    
    /*
     * Repaint all windows from Scratch
     */
    public void repaintAllFromScratch() {
        Enumeration<AWindow> myenum = windowsList.elements();
        
        hideNonVisible();
        
        while (myenum.hasMoreElements()) {
            AWindow window = (AWindow) myenum.nextElement();
            
            if (this.isReallyOnScreen(window)) {
                if (!window.isVisible()) {
                    window.setVisible(true);
                }
                window.repaintFromScratch();
            }
        }
        title.repaint();
    }
    
    /*
     * Return a string will the names of all layouts
     * to be shown in the layout dialog
     */
    public String[] getLayoutNames() {
        Enumeration<String> myenum = layouts.keys();
        String[] s = new String[layouts.size()];
        //Loop over all layouts and concatenate their names
        for (int i = 0; i < s.length; i++)
            s[i] = (String) myenum.nextElement();
        
        return s;
    }
    
    /*
     * Get Layout by name
     */
    public ALayout getLayout(String name) {
        if (layouts.containsKey(name))
            return layouts.get(name);
        else
            throw new Error("getLayout, unknown: " + name);
    }
    
    /*
     * Return the current layout
     */
    public ALayout getCurrentLayout() {
        return currentLayout;
    }
    
    /*
     * Set the current layout and apply the changes right away
     */
    public void setCurrentLayout(String newLayout) {
        //Make sure the new layout exists
        if (layouts.containsKey(newLayout)) {
            
            //get current layout and its dimension
            currentLayout = layouts.get(newLayout);
            Dimension dim = currentLayout.getSize();
            
            // clear the frame
            setLayout(null);
            removeAll();
            layeredPane.removeAll();
            
            //Toolkit is not aware of multi-screens, but "adds them all together".
            //  Dimension ScreenSize = Toolkit.getDefaultToolkit().getScreenSize();
            //getMaxWindowBounds gets max bounds, again added up over multiple screens. In contrast to the above,
            //it will account for MenuBars, StatusBars, etc from the the WindowManager
            //  Rectangle ScreenSize = GraphicsEnvironment.getLocalGraphicsEnvironment().getMaximumWindowBounds();
            //This is how to get the default screen, we should rather use the one we are on
            //  GraphicsEnvironment genv = GraphicsEnvironment.getLocalGraphicsEnvironment();
            //  GraphicsDevice gscreen = genv.getDefaultScreenDevice();
            //  GraphicsConfiguration gconf = gscreen.getDefaultConfiguration();
            
            //Final total Width, Height and position of the Canvas
            int W, H, X, Y;
            
            //First get the screen size of the current screen
            Rectangle ScreenSize = getScreenSize();
            
            //This is not a real full screen mode, rather a maximised Window mode.
            //In proper full screen, some window managers (such as Cacao) will allow
            //to have the title bar/menues disappear, etc.
            if(newLayout.equals("FULL SCREEN")) {
                //Really take the full screen in this mode
                W = ScreenSize.width;
                H = ScreenSize.height;
                X = ScreenSize.x;
                Y = ScreenSize.y;
            } else {
                //if not full screen then have to take account of task bars
                //First gets the insets (borders) from all sides
                Insets borders = new Insets(0, 0, 0, 0);
                if (!AGlobals.isAtlantisHeadless()) {
                    borders = Toolkit.getDefaultToolkit()
                            .getScreenInsets(parentWindow.getGraphicsConfiguration());
                }
                //calculate new usable screen size
                ScreenSize.width -= borders.left+borders.right;
                ScreenSize.height -= borders.top+borders.bottom;
                ScreenSize.x += borders.left;
                ScreenSize.y += borders.top;
                
                W = (int) (ScreenSize.width * SCREEN_USAGE);
                H = ScreenSize.height;
                X = ScreenSize.x;
                Y = ScreenSize.y;
            }
            
            //Apply the new layout dimensions
            layeredPane.setLayout(new AGridLayout(dim, null));
            
            //Now check for the existing windows
            String[] usedWindowNames = currentLayout.getWindowNames();
            
            //Loop over windows and add them to the new layout
            for (int i = 0; i < usedWindowNames.length; i++) {
                //Get the window dimensions
                Rectangle constraint = currentLayout.getWindowConstraints(usedWindowNames[i]);
                //Get the window itself
                AWindow window = windowsList.get(usedWindowNames[i]);
                //Make sure its finalized
                if (!window.finalized) {
                    window.finalizeConstruction();
                }
                //Add window to a new layer with the given dimensions
                layeredPane.setLayer(window, baseLayer + i);
                layeredPane.add(window, constraint);
            }
            // adding the layeredPane to the frame
            setLayout(new BorderLayout());
            add(layeredPane);
            
            if (parentWindow != null) {
                //Set the new window dimensions and position
                parentWindow.setLocation(X, Y);
                parentWindow.setSize(W, H);

                //Validate and update
                parentWindow.validate();
                parentWindow.repaint();
                
            } else {
                
                // This is real voodoo to make the canvas lay itself out in the
                // right way in headless mode
                // It seems that because there's no repaint events things like
                // layouts often don't work in the way you expect. I'm fully
                // willing to believe I don't understand the best way to do this.
                // This is rather a scattergun approach that seems to work...
                // - Adam
                
                setSize(W, H);
                layeredPane.setSize(W, H);
                
                Dimension d = layeredPane.getLayout().preferredLayoutSize(layeredPane);
                
                validate();
                repaint();
                
                setSize(d);
                layeredPane.setSize(d);
                layeredPane.getLayout().layoutContainer(layeredPane);
            }
            
            fireLayoutChange();
            
            //Now apply window ordering
            String[] ss = currentLayout.getStartupSequence();
            for (int i = 0; i < ss.length; i++)  this.moveToFrontWindow(ss[i]);
            
            //Finally set the current window to the same it was before, if it exists
            if ((currentWindow != null) && (currentLayout.hasWindow(currentWindow.getName())))
                setCurrentWindow(currentWindow.getName());
            else
                setCurrentWindow(currentLayout.getStartupWindow());
        } else
            throw new Error("setCurrentLayout: unknown layout name !?");
    }
    
    /*
     * Inform everyone on the LayoutChangeListener list about the layout change
     */
    private void fireLayoutChange() {
    	for (ALayoutChangeListener changeListener : layoutChangeListeners) {
    		changeListener.layoutChanged(getCanvas());
    	}
    }
    
    /*
     * Store the current layout in the stack
     * This is need for layout history, used by e.g. "Zoom/Unzoom LAr/Calorimeters"
     */
    public void saveLayout() {
        //Add the current layout and active window name
        windowLayoutHistory.push(getCurrentLayout());
        windowNameHistory.push(currentWindow.getName());
        
        //Also store all windows that are actually on screen
        Vector<String> windowsOnScreen = new Vector<String>();
        String[] usedWindowNames = currentLayout.getWindowNames();
        for (String windowName : usedWindowNames) {
        	AWindow window = windowsList.get(windowName);
        	if(isReallyOnScreen(window)) {
                windowsOnScreen.add(windowName);
            }
        }
        windowsActive.push(windowsOnScreen);
    }
    
    /*
     * Revert to the previous layout in the layout history
     */
    void unlayout() {
        //Make sure there is a previous one
        if(windowLayoutHistory.size() > 0) {
            //Get the old layout and active window
            ALayout newLayout=windowLayoutHistory.pop();
            String windowName=windowNameHistory.pop();
            //Only change layout if it has actually changed
            if(!newLayout.getName().equals(currentLayout.getName())) {
                //Set new layout and current window
                setCurrentLayout(newLayout.getName());
                setCurrentWindow(windowName);
            }
            //now bring windows in the proper order
            Vector<String> windowsOnScreen = windowsActive.pop();
            
            for (String winName : windowsOnScreen) {
            	this.moveToFrontWindow(winName);
            }
        }
    }
    
    /*
     * Add objects that shall be informed if there is a change in the lock status
     */
    public void addLockChangeListener(ChangeListener listener) {
        lockChangeListeners.addElement(listener);
        listener.stateChanged(new ChangeEvent(this));
    }
    /*
     * Add objects that shall be informed if there is a change in the layout
     */
    public void addLayoutChangeListener(ALayoutChangeListener listener) {
        layoutChangeListeners.addElement(listener);
        listener.layoutChanged(getCanvas());
    }
    
    /*
     * Add objects that shall be informed if there is a
     * change in the current window
     */
    public void addWindowChangeListener(ChangeListener listener) {
        windowChangeListeners.addElement(listener);
        listener.stateChanged(new ChangeEvent(this));
    }
    
    /*
     * Check if the given window name exists in the list of windows
     */
    public boolean isValidWindowName(String name) {
        return windowsList.containsKey(name);
    }
    
    /*
     * Get a string with all the existing windows
     */
    public String[] getKnownWindowNames() {
        Set<String> windowNames = windowsList.keySet();
        return windowNames.toArray(new String[windowNames.size()]);
    }
    
    /*
     * Returns the name of the current window
     */
    public String getCurrentWindowName() {
        return currentWindow.getName();
    }
    
    /*
     * Return the current window
     */
    public AWindow getCurrentWindow() {
        return currentWindow;
    }
    
    /*
     * Return the window we are currently painting in
     */
    public AWindow getPaintingWindow() {
        return paintingWindow;
    }
    
    /*
     * Notify everyone about a change in the active window
     */
    private void fireWindowChange() {
        ChangeEvent changeEvent = new ChangeEvent(this);
        for (ChangeListener changeListener : windowChangeListeners) {
            changeListener.stateChanged(changeEvent);
        }
    }
    
    /*
     * Notify everyone about a change in the lock status
     */
    void fireLockChange() {
        ChangeEvent changeEvent = new ChangeEvent(this);
        for (ChangeListener changeListener : lockChangeListeners) {
            changeListener.stateChanged(changeEvent);
        }
    }
    
    /*
     * Get number of windows
     */
    public int getWindowsCount() {
        return windowsList.size();
    }
    
    /**
     * Sets the current window. Informs all registered listeners.
     *
     * @param newWindowName The name of the window to be set to current.
     */
    public void setCurrentWindow(String newWindowName) {
        //Make sure window does exist
        if (windowsList.containsKey(newWindowName)) {
            //Deselect the current window
            if (currentWindow != null) currentWindow.deselect();
            //Get the new window
            currentWindow = (AWindow) windowsList.get(newWindowName);
            //Select it
            currentWindow.select();
            //Inform APar class about the new current window
            parameterStore.setCurrentWindowIndex(currentWindow.getIndex());
            //Move the current window on top of all others
            moveToFrontWindow(currentWindow.getName());
            //Inform everyone about the change
            fireWindowChange();
        } else
            throw new Error("setCurrentWindow, window: " + newWindowName + " , doesn't exist !?");
    }
    
    /**
     * Sets the currently window currently being painted
     *
     * @param window the window being painted
     */
    public void setPaintingWindow(AWindow window) {
        paintingWindow = window;
    }
    
    /**
     * Copy settings from one window to another window
     *
     * @param from the window to copy settings from
     * @param to the window to copy settings to
     */
    public void copyWindowSettings(String from, String to) {
        AWindow fromW = getWindow(from);
        AWindow toW = getWindow(to);
        
        parameterStore.copyParameters(from, to);
        
        // IMPORTANT! Do not change the order
        // setCurrentWindow() should be always before setProjection()
        setCurrentWindow(to);
        
        // copy the projection, group and scales
        toW.setProjection(fromW.getProjection());
        toW.setGroup(fromW.getGroupName());
        toW.setScaleStatus(fromW.getScaleStatus());
        
        // copy the interaction (no interactions valid for EventInfo projection)
        if(!(fromW.getProjection() instanceof AProjectionEventInfo))
        {
        	AInteractionToolBar fromI = fromW.getInteractionToolBar();
        	AInteractionToolBar toI = toW.getInteractionToolBar();
        
        	toI.setSelectedGroup(fromI.getSelectedGroupName());
        	// copy the state of the panel
        	toI.getSelectedGroup().setGroupState(fromI.getSelectedGroup());
        }
        // for LegoPlot windows we need to set the (x,y,z)-corners from the projection
        // TODO: Check and understand this. Previously there was no check that toW was
        //       an instance of AProjectionLegoPlot, put perhaps this works because only
        //       ever copy settings from one projection to another of the same type?
        if(fromW.getProjection() instanceof AProjectionLegoPlot
        		&& toW.getProjection() instanceof AProjectionLegoPlot) {
            AProjectionLegoPlot.setxz(toW.getIndex(), AProjectionLegoPlot.getxz(fromW.getIndex()));
            AProjectionLegoPlot.setyz(toW.getIndex(), AProjectionLegoPlot.getyz(fromW.getIndex()));
            //reset the center of detector dot on ZMR
            toW.getInteractionManager().setContext(toW.getInteractionToolBar().getSelectedGroup());
        }
        //copy the user corners
        toW.setUserCorners(fromW.getUserCorners());
    }
    
    /**
     * As each window has its own layer, find the window that is in this window
     * 
     * @param layer the layer which the window is in
     */
    private AWindow getWindowForLayer(int layer) {
        //Get all the components in this layer
        Component[] c = layeredPane.getComponentsInLayer(layer);
        //Return the first instance of AWindow
        for (int i = 0; i < c.length; i++)
            if (c[i] instanceof AWindow) return (AWindow) c[i];
        //Otherwise return 0
        return null;
    }
    
    /**
     * Get the associated tile menu bar
     */
    public ATitleMenuBar getATitle() {
        return title;
    }
    
    /**
     * Get the top-most window at a certain point
     *
     * @param x x-coordinate in pixels
     * @param y y-coordinate in pixels
     */
    private AWindow getWindowForPoint(double x, double y) {
        //Loop over all layers
        int maxLayer = baseLayer + currentLayout.getWindowNames().length;
        for (int lay = maxLayer; lay >= 0; lay--) {
            //Get the window that is in this layer
            AWindow w = getWindowForLayer(lay);
            if (w == null || currentLayout == null) {
                continue;
            }
            //Get the window rectangle
            Rectangle r = currentLayout.getWindowConstraints(w.getName());
            //Check if x,y is inside the rectangle
            if (r.contains(x, y))
                return w;
        }
        
        return null;
    }
    
    /**
     * Check if a given window is partially visible
     * 
     * @param window the window to check
     */
    public boolean isReallyOnScreen(AWindow window) {
        //The full area covered by the canvas
        Dimension layoutSize = currentLayout.getSize();
        
        //Run over every single point on the canvas 
        //until we hit one that belongs to the window -- aaaaaarrrrggghhh!!! (SB?)
        
        //This function almost always exits on the first pixel
        //Profiling indicates it takes only a few miliseconds
        //even in very rare worst cases - AD
        for (int j = 0; j < layoutSize.height; j++)
            for (int i = 0; i < layoutSize.width; i++)
                if (getWindowForPoint(i + 0.5, j + 0.5).equals(window))
                    return true;
        
        return false;
    }

  /**
   * Implementation of the new event listener: gets called for each new event
   * and repaints all the windows.
   * 
   * @param evt the new event
   */
    @Override
    public void newEvent(AEvent evt) {
    	repaintAllFromScratch();
    }
    
    /** Return the aspect ratio of the canvas drawing area (hopefully) as h/w.
     *  Sorry if this is usually defined as w/h? - AD */
    public double getAspectRatio() {
        Dimension d = layeredPane.getSize();
        return d.getHeight() / d.getWidth();
    }

   /**
    * Calculates the image height that will result if one resizes the current canvas to the given width.
    * @param width target width
    * @return corresponding height
    */
    public int getRespectiveHeight(int width){

      //Get rootpane dimensions
      int canvasWidth = getWidth();
      int canvasHeight = getHeight();

      //calculate factor
      double factor = ((double)width) / canvasWidth;

      //Account for menu bar if its there
      if (title != null && parameterStore.get("Prefs", "CanvasTitle").getStatus()) {
      //if (getTitleBar() != null && getTitleBar().isVisible()){
            int height = title.getHeight();

            if (AGlobals.isAtlantisHeadless()) {
                // In headless mode the menu bar isn't really enabled
                // So get the size it would be if it was
                height = (int) (title.getLayout().preferredLayoutSize(title).getHeight());
            }

            return (int) (factor * canvasHeight) + height;

        }

      //Otherwise just scale
      return (int)(factor*canvasHeight);
   }

} //ACanvas
