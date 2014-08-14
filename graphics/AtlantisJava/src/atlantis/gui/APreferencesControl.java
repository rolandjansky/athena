package atlantis.gui;

import java.io.File;
import java.awt.Frame;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JMenu;
import javax.swing.JOptionPane;
import javax.swing.JSeparator;

import atlantis.canvas.ACanvas;
import atlantis.canvas.ALegendWindow;
import atlantis.globals.AGlobals;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.APar;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;


/**
 * Preferences menu item
 *
 * @author Gary Taylor, Zdenek Maxa
 */
public class APreferencesControl extends JMenu implements ActionListener, ItemListener
{
	private static ALogger logger = ALogger.getLogger(APreferencesControl.class);
	
	private static AGlobals globals = AGlobals.instance();
	
	protected static APar parameterStore = APar.instance();
	
    private static final String LAYOUT = "Change Canvas Layout";
    private static final String SELECT_COLOR_MAP = "Select Color Map";
    private static final String COLOR_MAP_EDITOR = "Color Map Editor";
    private static final String READ_COLOR_MAP = "Read Color Map";
    private static final String WRITE_COLOR_MAP = "Save Current Color Map";
    private static final String WRITE_CONFIG = "Save Current Configuration";
    private static final String POINTER_POSITION = "Show Pointer Position Window";
    private static final String LEGEND = "Show Legend Window";
    private static final String CANVAS_TITLE = "Show Canvas Title";
    private static final String WINDOW_TITLES = "Show Window Titles";
    private static final String FISHEYE_LOGO = "Show Fisheye Indicator";
    private static final String HIDE_SCALES = "Hide Scales";
    private static final String ATLAS_LOGO = "Show ATLAS logo on saved images";
    private static final String ANTI_ALIASING = "Use Anti-Aliasing";
    private static final String OPEN_GL = "Use OpenGL";

    private static JCheckBoxMenuItem posMenuItem;
    private static JCheckBoxMenuItem legMenuItem;
    private static JCheckBoxMenuItem canvasTitleMenuItem;
    private static JCheckBoxMenuItem windowTitleMenuItem;
    private static JCheckBoxMenuItem fisheyeIndicatorMenuItem;
    private static JCheckBoxMenuItem hideScalesMenuItem;
    private static JCheckBoxMenuItem atlasLogoMenuItem;
    private static JCheckBoxMenuItem aliasMenuItem;
    private static JCheckBoxMenuItem openglMenuItem;

    // ----------------------------------------------------------------------

    public APreferencesControl()
    {
        super("Preferences");
        this.setToolTipText("Preferences");

        add(SELECT_COLOR_MAP).addActionListener(this);
        add(COLOR_MAP_EDITOR).addActionListener(this);
        add(READ_COLOR_MAP).addActionListener(this);
        add(WRITE_COLOR_MAP).addActionListener(this);

        add(new JSeparator());

        add(LAYOUT).addActionListener(this);
        add(WRITE_CONFIG).addActionListener(this);

        add(new JSeparator());

        posMenuItem = new JCheckBoxMenuItem(POINTER_POSITION, false);
        posMenuItem.addItemListener(this);
        add(posMenuItem);

        legMenuItem = new JCheckBoxMenuItem(LEGEND, false);
        legMenuItem.addItemListener(this);
        add(legMenuItem);

        add(new JSeparator());
        
        canvasTitleMenuItem = new JCheckBoxMenuItem(CANVAS_TITLE, parameterStore.get("Prefs", "CanvasTitle").getStatus());
        canvasTitleMenuItem.addItemListener(this);
        add(canvasTitleMenuItem);

        windowTitleMenuItem = new JCheckBoxMenuItem(WINDOW_TITLES, parameterStore.get("Prefs", "WindowTitle").getStatus());
        windowTitleMenuItem.addItemListener(this);
        add(windowTitleMenuItem);

        fisheyeIndicatorMenuItem = new JCheckBoxMenuItem(FISHEYE_LOGO, parameterStore.get("Prefs", "FisheyeIndicator").getStatus());
        fisheyeIndicatorMenuItem.addItemListener(this);
        add(fisheyeIndicatorMenuItem);
        
        hideScalesMenuItem = new JCheckBoxMenuItem(HIDE_SCALES, parameterStore.get("Prefs", "HideScales").getStatus());
        hideScalesMenuItem.addItemListener(this);
        add(hideScalesMenuItem);
        
        atlasLogoMenuItem = new JCheckBoxMenuItem(ATLAS_LOGO, parameterStore.get("Prefs", "AtlasLogo").getStatus());
        atlasLogoMenuItem.addItemListener(this);
        add(atlasLogoMenuItem);
       
        
        add(new JSeparator());
        
        aliasMenuItem = new JCheckBoxMenuItem(ANTI_ALIASING, parameterStore.get("Prefs", "AntiAlias").getStatus());
        aliasMenuItem.addItemListener(this);
        add(aliasMenuItem);
        
        openglMenuItem = new JCheckBoxMenuItem(OPEN_GL, parameterStore.get("Prefs", "OpenGL").getStatus());
        openglMenuItem.addItemListener(this);
        
        if (globals.getUseOpenGL()) {
            add(new JSeparator());
            add(new AOpenGLControl());
        }
    } // APreferencesControl() ----------------------------------------------



    public void itemStateChanged(ItemEvent e)
    {
        if(((JCheckBoxMenuItem)e.getSource()).getText().equals(CANVAS_TITLE))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
            {
                parameterStore.get("Prefs", "CanvasTitle").setStatus(true);
                ACanvas.getCanvas().getTitleBar().setVisible(true);
            }
            else
            {
                parameterStore.get("Prefs", "CanvasTitle").setStatus(false);
                ACanvas.getCanvas().getTitleBar().setVisible(false);
            }
            ACanvas.getCanvas().validate();
            ACanvas.getCanvas().repaint();
        }
        else if(((JCheckBoxMenuItem)e.getSource()).getText().equals(POINTER_POSITION))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
                APointerPositionWindow.getInstance().setVisible(true);
            else
                APointerPositionWindow.getInstance().dispose();
        }
        else if(((JCheckBoxMenuItem)e.getSource()).getText().equals(LEGEND))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
            {
                ALegendWindow legendWindow = ALegendWindow.getInstance();
                // Want to deselect legend menu item if legend window is closed
                legendWindow.addWindowListener(new WindowAdapter() {
                	@Override
                	public void windowClosed(WindowEvent e) {
                		setLegMenuItem(false);
                	}
                });

                legendWindow.setVisible(true);
                ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
            }
            else
                ALegendWindow.getInstance().dispose();
        }
        else if(((JCheckBoxMenuItem)e.getSource()).getText().equals(WINDOW_TITLES))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
                parameterStore.get("Prefs", "WindowTitle").setStatus(true);
            else
                parameterStore.get("Prefs", "WindowTitle").setStatus(false);
            ACanvas.getCanvas().repaintAllFromScratch();
        }
        else if(((JCheckBoxMenuItem)e.getSource()).getText().equals(FISHEYE_LOGO))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
                parameterStore.get("Prefs", "FisheyeIndicator").setStatus(true);
            else
                parameterStore.get("Prefs", "FisheyeIndicator").setStatus(false);
            ACanvas.getCanvas().repaintAllFromScratch();
        }
        else if(((JCheckBoxMenuItem)e.getSource()).getText().equals(HIDE_SCALES))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
                parameterStore.get("Prefs", "HideScales").setStatus(true);
            else
                parameterStore.get("Prefs", "HideScales").setStatus(false);
            
            ACanvas.getCanvas().repaintAllFromScratch();
        }
        else if(((JCheckBoxMenuItem)e.getSource()).getText().equals(ATLAS_LOGO))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
                parameterStore.get("Prefs", "AtlasLogo").setStatus(true);
            else
                parameterStore.get("Prefs", "AtlasLogo").setStatus(false);
        }
        else if(((JCheckBoxMenuItem)e.getSource()).getText().equals(ANTI_ALIASING))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
                parameterStore.get("Prefs", "AntiAlias").setStatus(true);
            else
                parameterStore.get("Prefs", "AntiAlias").setStatus(false);
            ACanvas.getCanvas().repaintAllFromScratch();
        }
        else if(((JCheckBoxMenuItem)e.getSource()).getText().equals(OPEN_GL))
        {
            if(e.getStateChange() == ItemEvent.SELECTED)
                parameterStore.get("Prefs", "OpenGL").setStatus(true);
            else
                parameterStore.get("Prefs", "OpenGL").setStatus(false);
            System.out.println("No effect at present");
        }
    } // itemStateChanged() -------------------------------------------------



    public void actionPerformed(ActionEvent e)
    {
        String action = e.getActionCommand();

        if(LAYOUT.equals(action))
        {
            new ALayoutDialog();
        }
        else if(SELECT_COLOR_MAP.equals(action))
        {
            new AColorMapDialog();
        }
        else if(COLOR_MAP_EDITOR.equals(action))
        {
            AColorMapEditor.getInstance().setVisible(true);
        }
        else if(READ_COLOR_MAP.equals(action))
        {
            new AReadColorMapChooser().showMyDialog();
        }
        else if(WRITE_COLOR_MAP.equals(action))
        {
            writeColorMap();
        }
        else if(WRITE_CONFIG.equals(action))
        {
            writeConfiguration();
        }

    } // actionPerformed() --------------------------------------------------



    private static void writeColorMap()
    {
        String fileName = ".Atlantis-colormap.xml";
        String title = "Saving current color map";
        Frame gui = globals.getGuiFrame();

        logger.debug("Going to write customised color map file ...");
        String data = AColorMap.getColorMapXML();

        // color map data should be ready and valid - store it into file
        try
        {
            AGUIUtilities.chooseFileAndWrite(gui, System.getProperty("user.home"),
                    fileName, data.getBytes(), title, "xml");
            logger.debug("Current color map written (or cancelled by user).");
        }
        catch(AAtlantisException ex)
        {
            JOptionPane.showMessageDialog(gui, ex.getMessage(),
                    title, JOptionPane.ERROR_MESSAGE);
            logger.debug(ex.getMessage(), ex);
        }

    } // writeColorMap() ----------------------------------------------------



    private static void writeConfiguration()
    {
    	String FILE_SEPAR = System.getProperty("file.separator");
        String fileName = globals.getUserConfigFile();
        String title = "Saving current configuration";
        String fileNameFull = System.getProperty("user.home") + FILE_SEPAR + fileName;
        String sourceFileNameFull = "";
        Frame gui = globals.getGuiFrame();
        
        logger.debug("Writing customised configuration ...");

        // get currently used configuration file and data from it
        File user = new File(fileNameFull);
        if(user.canRead())
        {
            sourceFileNameFull = fileNameFull;
        	logger.debug("User has already got " + fileNameFull + 
        			     " this will be used as source.");
        }
        else
        {
            sourceFileNameFull = globals.getHomeDirectory() +
                                 "configuration" + FILE_SEPAR +
                                 "config.xml";
            logger.debug(sourceFileNameFull + " will be used as source.");
            
        }

        String data = AConfigWriter.getConfiguration(sourceFileNameFull);
        if(data == null)
        {
            String msg = "Error when reading source configuration from file\n" +
                         sourceFileNameFull;
            logger.error(msg);
            JOptionPane.showMessageDialog(gui, msg, title,
                    JOptionPane.INFORMATION_MESSAGE);
            return;
        }

        // configuration data should be ready and valid - store it into file
        try
        {
        	logger.debug("Current customised configuration made, going to save it ...");
            AGUIUtilities.chooseFileAndWrite(gui, System.getProperty("user.home"),
                    fileName, data.getBytes(), title, "xml");
            logger.debug("Saving customised configuration successful (or cancelled by user).");
        }
        catch(AAtlantisException ex)
        {
        	String m = ex.getMessage();
            JOptionPane.showMessageDialog(gui, m, title,
            		                      JOptionPane.ERROR_MESSAGE);
            logger.error(m);
            logger.debug(m, ex);
        }

    } // writeConfiguration() -----------------------------------------------

    public static void setPosMenuItem(boolean selected)
    {
        posMenuItem.setSelected(selected);
    }

    public static void setLegMenuItem(boolean selected)
    {
        legMenuItem.setSelected(selected);
    }
    
    public static void setWindowTitleMenuItem(boolean selected)
    {
        windowTitleMenuItem.setSelected(selected);
        parameterStore.get("Prefs", "WindowTitle").setStatus(selected);
    }
    
    public static void setFisheyeIndicatorMenuItem(boolean selected)
    {
        fisheyeIndicatorMenuItem.setSelected(selected);
        parameterStore.get("Prefs", "FisheyeIndicator").setStatus(selected);
    }
    
    public static void setHideScalesMenuItem(boolean selected)
    {
    	hideScalesMenuItem.setSelected(selected);
    	parameterStore.get("Prefs", "HideScales").setStatus(selected);
	}
    
    public static void setAtlasLogoMenuItem(boolean selected)
    {
        atlasLogoMenuItem.setSelected(selected);
        parameterStore.get("Prefs", "AtlasLogo").setStatus(selected);
    }
    
    public static void setAliasMenuItem(boolean selected)
    {
        if (!AGlobals.isAtlantisHeadless()) {
            aliasMenuItem.setSelected(selected);
        }
        parameterStore.get("Prefs", "AntiAlias").setStatus(selected);
    }

    public static boolean getWindowTitleMenuItem()
    {
        if(windowTitleMenuItem!=null)
            return windowTitleMenuItem.isSelected();
        else
            return parameterStore.get("Prefs", "WindowTitle").getStatus();
    }
    
    public static boolean getFisheyeIndicatorMenuItem()
    {
        if(fisheyeIndicatorMenuItem!=null)
            return fisheyeIndicatorMenuItem.isSelected();
        else
            return parameterStore.get("Prefs", "FisheyeIndicator").getStatus();
    }
    
    public static boolean getHideScalesMenuItem()
    {
    	if(hideScalesMenuItem!=null)
    		return hideScalesMenuItem.isSelected();
    	else
    		return parameterStore.get("Prefs", "HideScales").getStatus();
    }
    
    
    public static boolean getAtlasLogoMenuItem()
    {
        if(atlasLogoMenuItem!=null)
            return atlasLogoMenuItem.isSelected();
        else
            return parameterStore.get("Prefs", "AtlasLogo").getStatus();
    }
    
    public static boolean getAliasMenuItem()
    {
        return parameterStore.get("Prefs", "AntiAlias").getStatus();
        /*if(aliasMenuItem!=null)
            return aliasMenuItem.isSelected();
        else
            return false;*/
    }

} // class APreferencesControl ==============================================
