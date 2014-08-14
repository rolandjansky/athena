package atlantis.gui;

import java.awt.BorderLayout;

import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;

import java.net.Authenticator;
import java.util.List;

import atlantis.canvas.ACanvas;
import atlantis.canvas.ALayoutChangeListener;
import atlantis.canvas.AWindow;
import atlantis.event.AEventInfoPrinter;
import atlantis.event.AEventManager;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;
import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.graphics.dnd.ADnDLabel;
import atlantis.graphics.dnd.ADragListener;
import atlantis.list.AListManager;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.ACommandProcessor;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterChangeListener;
import atlantis.parameters.AParameterSuperGroup;
import atlantis.parameters.AParametersGroup;
import atlantis.projection.AProjection;
import atlantis.projection.AProjectionsManager;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;
import javax.swing.*;

/**
 * The Atlantis Graphical User Interface
 * This is a singleton class (and probably one of the few cases where this is
 * justified!). By implementing the ALayoutChangeListener it adjusts its size to
 * the canvas size.
 */
public class AGUI extends JFrame implements ChangeListener,
ALayoutChangeListener,
AParameterChangeListener
{
    private final static ALogger logger = ALogger.getLogger(AGUI.class);
    private final static AGlobals globals = AGlobals.instance();
    public AEventManager eventManager = AEventManager.instance();

    private static AGUI instance;

    private static APar parameterStore = APar.instance();

    /** Limit the total width of Atlantis to this number of pixels (0 = screen width). */
    private int maxWidth = 0;

    private AItemTabbedPane[] tabbedPane;
    private AParametersPage[][] pages;
    private AParametersGroup[][] groups;
    private ChangeListener[] tabChangeListener;
    private ChangeListener outerTabChangeListener;
    private JPanel panel;
    private JSplitPane split;
    private JTabbedPane outerTabbedPane;
    private AInteractionToolBar interactionToolBar;

    /**
     * Public singleton instantian-accessor
     * @return the singleton instance
     */
    public static AGUI getGUI()
    {
        //Don't create any GUI in headless mode
        if (AGlobals.isAtlantisHeadless()) return null;
        //Create the GUI if it does not exist
        if (instance == null) {
            instance = new AGUI();
            globals.setGuiFrame(instance);
        }
        //return the singleton instance
        return instance;
    }

    /**
     * Private singleton constructor
     */
    private AGUI()
    {
        //intialize parent
        super("Atlantis GUI");

        //be verbose
        logger.debug("Creating GUI...");

        // AClosingConfirmationDialog class handles closing Atlantis
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        this.addWindowListener(new AClosingConfirmationDialog(this));

        getContentPane().setLayout(new BorderLayout());
        AIcon.setIconImage(this);

        // Creating all menu bar items
        JMenuBar menuBar = constructAndGetMenuBar();
        setJMenuBar(menuBar); // set menubar to this frame

        // Create a new toolbar for the event sources
        AEventSourceToolBar toolBar = new AEventSourceToolBar();
        toolBar.setName("Atlantis event source toolbar");
        //Does not go into main panel as it need BorderLayout
        add(toolBar, BorderLayout.PAGE_START);
        // register as an event source listener
        eventManager.addNewEventSourceListener(toolBar);

        // building the panel with our own layout manager
        // Why do we actually need it??
        panel = new JPanel(new AGUILayoutManager());

        // setting the Window Control
        AWindowControl windowControl = new AWindowControl();
        windowControl.setName("windowControl");
        panel.add(windowControl, AGUILayoutManager.AVAILABLExPREFERRED);

        // setting the Parameters Table
        groups = parameterStore.getUIGroups();
        pages = new AParametersPage[groups.length][];
        tabbedPane = new AItemTabbedPane[groups.length];
        tabChangeListener = new TabChangeListener[groups.length];

        for(int i = 0; i < groups.length; i++)
        {
            pages[i] = new AParametersPage[groups[i].length];
            tabbedPane[i] = new AItemTabbedPane(JTabbedPane.LEFT);

            for(int j = 0; j < groups[i].length; j++)
            {
                pages[i][j] = new AParametersPage(groups[i][j]);
                String title = groups[i][j].getScreenName();
                tabbedPane[i].addTab(title, null,
                        pages[i][j], groups[i][j].getToolTip());
                pages[i][j].setName(title);
                if(groups[i][j].getScreenName().equals("Residual") || 
                        groups[i][j].getScreenName().equals("Physics") )
                    tabbedPane[i].setEnabledAt(tabbedPane[i].getTabCount()-1, false);
            }
            tabbedPane[i].setSelectedIndex(-1);
            tabChangeListener[i] = new TabChangeListener();
            tabbedPane[i].addChangeListener(tabChangeListener[i]);
            tabbedPane[i].addMouseListener(new MouseAdapter()
            {
                public void mousePressed(MouseEvent e)
                {
                    if(AUtilities.isRightMouseButton(e))
                    {
                        JTabbedPane tabbedPane = (JTabbedPane) e.getSource();
                        int tab = tabbedPane.getUI().tabForCoordinate(tabbedPane, e.getX(), e.getY());
                        int parentTab = ((JTabbedPane) tabbedPane.getParent()).getSelectedIndex();
                        if(tab != -1)
                        {
                            String helpName = groups[parentTab][tab].
                                    getGroupName();
                            AHelpSystem.getInstance().showPage(helpName);
                        }
                    }
                }
            });
        }
        // for loop - setting the outer Table

        outerTabbedPane = new AItemTabbedPane(JTabbedPane.TOP);
        outerTabbedPane.setName("parameterGroups");
        List<AParameterSuperGroup> superGroups = parameterStore.getUISuperGroups();
        for(int i = 0; i < superGroups.size(); i++) {
            AParameterSuperGroup superGroup = superGroups.get(i);
            String title = superGroup.getName();
            String toolTip = superGroup.getToolTip();
            outerTabbedPane.addTab(title, null, tabbedPane[i], toolTip);
            tabbedPane[i].setName(title);
        }
        outerTabChangeListener = new OuterTabChangeListener();
        outerTabbedPane.addChangeListener(outerTabChangeListener);
        outerTabbedPane.addMouseListener(new MouseAdapter()
        {
            public void mousePressed(MouseEvent e)
            {
                if(AUtilities.isRightMouseButton(e))
                {
                    JTabbedPane tabbedPane = (JTabbedPane) e.getSource();
                    int tab = tabbedPane.getUI().tabForCoordinate(tabbedPane, e.getX(), e.getY());
                    if(tab != -1)
                    {
                        String helpName = tabbedPane.getTitleAt(tab);
                        AHelpSystem.getInstance().showPage(helpName);
                    }
                }
            }
        });
        panel.add(outerTabbedPane, AGUILayoutManager.AVAILABLExAVAILABLE);

        // Set up the output (log) pane.
        AMainLogPane outputDisplay = new AMainLogPane();
        // Make it available to all
        AOutput.setOutput(outputDisplay);
        // Display details of each new event
        AEventInfoPrinter eventInfoPrinter = new AEventInfoPrinter(outputDisplay);
        eventManager.addNewEventListener(eventInfoPrinter);
        // Put it in the GUI
        split = new JSplitPane(JSplitPane.VERTICAL_SPLIT, panel, outputDisplay);
        split.setOneTouchExpandable(true);
        split.setResizeWeight(0.75); //Give 25% to the log pane by default
        getContentPane().add(split);

        // setting the window, projection and group listeners
        ACanvas.getCanvas().addWindowChangeListener(this);
        String[] wName = ACanvas.getCanvas().getKnownWindowNames();
        for(int i = 0; i < wName.length; i++) {
            AWindow w = ACanvas.getCanvas().getWindow(wName[i]);
            w.addProjectionChangeListener(new ChangeListener() {
                public void stateChanged(ChangeEvent e) {
                    AWindow window = (AWindow) e.getSource();
                    if(window.equals(ACanvas.getCanvas().getCurrentWindow())) {
                        if(interactionToolBar != null) panel.remove(interactionToolBar);
                        interactionToolBar = window.getInteractionToolBar();
                        if(interactionToolBar == null)
                            window.getInteractionManager().forgetContext();
                        else {
                            // the interactions tabpanes are added here
                            panel.add(interactionToolBar, AGUILayoutManager.AVAILABLExPREFERRED, 0);
                            interactionToolBar.connect();
                            validate();
                        }
                    }
                }
            });

            w.addGroupChangeListener(new ChangeListener() {
                public void stateChanged(ChangeEvent e) {
                    AWindow window = (AWindow) e.getSource();
                    if(window.equals(ACanvas.getCanvas().getCurrentWindow()))
                        setSelectedTab(window.getGroupName());
                }
            });
        }

        // Set Authenticator for password-protected URL access to use
        // APasswordDialog.
        Authenticator.setDefault(APasswordDialog.getAuthenticator(this));

    } // AGUI() -------------------------------------------------------------



    /**
     * Limit the total width of Atlantis to this number of pixels.
     *
     * @param maxWidth the maximum allowed width in pixels (0 = screen width)
     */
    public void setMaxWidth(int maxWidth) {
        this.maxWidth = maxWidth;
    }

    /**
     * Constructs all menu bar (master) items and returns its instance
     * @return JMenuBar
     */
    private JMenuBar constructAndGetMenuBar()
    {
        JMenuBar menuBar = new JMenuBar();
        menuBar.add(new AFileControl()); // File menu bar item
        menuBar.add(new APreferencesControl()); // Preferences menu bar item
        menuBar.add(new AListsControl()); // List manager

        // Spacing, remaining buttons are aligned to the right
        menuBar.add(Box.createHorizontalGlue());

        // Reset button
        AMenuButton defaults = new AMenuButton("Reset");
        defaults.setToolTipText("Reload default settings");
        defaults.setBorder(null);
        defaults.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                restoreDefaults();
            }
        });
        menuBar.add(defaults);

        // Demo button
        AMenuButton demoButton = new AMenuButton("Demo");
        demoButton.setToolTipText("Start Demo mode");
        demoButton.setBorder(null);
        demoButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ADemoDialog.getInstance().setVisible(true);
            }
        });
        menuBar.add(demoButton);

        // Help menu item
        menuBar.add(new AHelpControl());

        return menuBar;

    } // constructAndGetMenuBar() -------------------------------------------



    /**
     * This gets called when the current layout changes in ACanvas
     */
    public void layoutChanged(ACanvas canvas) {

        // adapt the width of the GUI
        Rectangle screenSize = canvas.getScreenSize();

        int useWidth = screenSize.width;
        if (maxWidth > 0 && useWidth > maxWidth) {
            useWidth = maxWidth;
        }

        Rectangle canvbound = canvas.getStartupCanvasSize();
        int guiHeight = canvbound.height;
        if (canvas.getCurrentLayout().getName().equals("FULL SCREEN")) {
            int guiWidth = useWidth / 4;
            setBounds(useWidth * 3 / 4, 0, guiWidth, guiHeight);
            // minimise the gui in fullscreen mode
            int state = getExtendedState();
            state |= JFrame.ICONIFIED;
            setExtendedState(state);
        } else {
            int guiWidth = useWidth - canvbound.width - canvbound.x;
            setBounds(canvbound.x + canvbound.width, canvbound.y, guiWidth, guiHeight);
        }
    } // layoutChanged() -----------------------------------------------------

    /**
     * This gets called when the current window changes in ACanvas
     * @param e The ChangeEvent from the Window
     */
    public void stateChanged(ChangeEvent e)
    {
        setSelectedTab(ACanvas.getCanvas().getCurrentWindow().getGroupName());
        if(interactionToolBar != null) panel.remove(interactionToolBar);
        interactionToolBar = ACanvas.getCanvas().getCurrentWindow().getInteractionToolBar();
        // if there is an InteractionToolBar for this window (and projection)
        // add it to the GUI.
        if(interactionToolBar != null)
        {
            interactionToolBar.setName("Atlantis interaction toolbar");
            panel.add(interactionToolBar, AGUILayoutManager.AVAILABLExPREFERRED, 0);
            interactionToolBar.connect();
        }
        validate();
    } // stateChanged() -----------------------------------------------------



    private void setSelectedTab(String newTabName)
    {
        for(int i = 0; i < groups.length; i++)
            for(int j = 0; j < groups[i].length; j++)
                if(groups[i][j].getGroupName().equals(newTabName))
                {
                    for(int k = 0; k < groups.length; k++)
                        if(k != i)
                        {
                            if(tabbedPane[k].getSelectedIndex() != -1)
                            {
                                tabbedPane[k].removeChangeListener(tabChangeListener[k]);
                                tabbedPane[k].setSelectedIndex( -1);
                                tabbedPane[k].addChangeListener(tabChangeListener[k]);
                            }
                        }
                    tabbedPane[i].removeChangeListener(tabChangeListener[i]);
                    tabbedPane[i].setSelectedIndex(j);
                    tabbedPane[i].addChangeListener(tabChangeListener[i]);
                    outerTabbedPane.setSelectedIndex(i);
                    if(pages[i][j].pTable != null)
                        pages[i][j].pTable.refresh();
                    break;
                }
    } // setSelectedTab() ---------------------------------------------------



    public String getCurrentGroup()
    {
        return ACanvas.getCanvas().getCurrentWindow().getGroupName();
    } // getCurrentGroup() --------------------------------------------------



    public void refresh()
    {
        if(groups != null)
        {
            for(int i = 0; i < groups.length; i++)
                for(int j = 0; j < groups[i].length; j++)
                    if(pages[i][j].pTable != null)
                        pages[i][j].pTable.refresh();
        }
    } // refresh() ----------------------------------------------------------


    public void repaintTable()
    {
        if(tabbedPane != null)
            for(int i = 0; i < tabbedPane.length; i++)
                if(tabbedPane[i] != null)
                    tabbedPane[i].repaint();
    } // repaintTable() -----------------------------------------------------



    /**
     * Restore all the default configuration
     */
    public void restoreDefaults()
    {
        //restore default parameters
        parameterStore.restoreDefaults();
        //reset primary vertex in current event if there is one
        if (eventManager.getCurrentEvent() != null) {
            eventManager.getCurrentEvent().setPrimaryVertex();
        }
        //reset all list
        AListManager.getInstance().reset();
        //Finally refresh GUI and canvas
        this.refresh();
        ACanvas.getCanvas().restoreDefaults();

    } // restoreDefaults() --------------------------------------------------



    class TabChangeListener implements ChangeListener
    {
        public void stateChanged(ChangeEvent e)
        {
            JTabbedPane tabbedPane = (JTabbedPane) e.getSource();
            int index = tabbedPane.getSelectedIndex();
            if(index > -1)
            {
                int parentIndex = ((JTabbedPane) tabbedPane.getParent()).
                        getSelectedIndex();
                String name = groups[parentIndex][index].getGroupName();
                ACommandProcessor.receive(name + ".");
                pages[parentIndex][index].repaint();
                pages[parentIndex][index].pTable.refresh();
                if(groups[parentIndex][index].getUIUsage() ==
                        AParametersGroup.PROJECTION)
                {
                    AProjection p = AProjectionsManager.getProjection(name);
                    ACanvas.getCanvas().getCurrentWindow().setProjection(p);
                }
            }

        } // stateChanged() -------------------------------------------------

    } // class TabChangeListener ============================================



    class OuterTabChangeListener implements ChangeListener
    {
        public void stateChanged(ChangeEvent e)
        {
            JTabbedPane tabbedPane = (JTabbedPane) e.getSource();
            int parentIndex = tabbedPane.getSelectedIndex();
            if(parentIndex > -1)
            {
                JTabbedPane child = (JTabbedPane) tabbedPane.getSelectedComponent();
                if(child.getTabCount() == 0)
                {
                    child.setSelectedIndex( -1);
                }
                else
                {
                    int index = 0;
                    String name = groups[parentIndex][index].getGroupName();
                    // for projections don't change the current one
                    if(groups[parentIndex][index].getUIUsage() ==
                            AParametersGroup.PROJECTION)
                    {
                        name = ACanvas.getCanvas().getCurrentWindow().getProjection().
                                getName();
                        for(int i = 0; i < groups[parentIndex].length; ++i)
                        {
                            if(groups[parentIndex][i].getGroupName().equals(name))
                                index = i;
                        }
                    }
                    ACommandProcessor.receive(name + ".");
                    pages[parentIndex][index].repaint();
                    pages[parentIndex][index].pTable.refresh();
                    if(groups[parentIndex][index].getUIUsage() ==
                            AParametersGroup.PROJECTION)
                    {
                        AProjection p = AProjectionsManager.getProjection(name);
                        ACanvas.getCanvas().getCurrentWindow().setProjection(p);
                    }
                }
            }

        } // stateChanged() -------------------------------------------------

    } // class OuterTabChangeListener =======================================



    static class TabDragListener implements ADragListener
    {
        public void dragPerformed(Object from, Object to, int action)
        {
            String wName;
            if(to instanceof ADnDLabel)
                wName = ((ADnDLabel) to).getText();
            else if(to instanceof AWindow)
                wName = ((AWindow) to).getName();
            else
                wName = "???";
            JTabbedPane tp = (JTabbedPane) from;
            String tabName = tp.getTitleAt(tp.getSelectedIndex());
            AOutput.append("Drag of " + tabName + " into window " + wName + "\n",
                    ALogInterface.COMMAND);
            parameterStore.copyCurrentGroupLocalsTo(wName);
            ACanvas.getCanvas().getWindow(wName).repaintFromScratch();

        } // dragPerformed() ------------------------------------------------

    } // class TabDragListener ==============================================

    @Override
    public void parameterChanged(AParameter parameter) {
        this.repaintTable();

        if(parameter.getName().equals("Mode")){
            int calomode = parameterStore.get("LegoPlot", "Mode").getI();

            //set different nphicells and netacells combinations depending on the <calo view> selection as it is used in the L1Calo configuration file 
            if (calomode== 0){
                parameterStore.get("LegoPlot", "nphicells").setI(128);
                parameterStore.get("LegoPlot", "netacells").setI(200);
            }
            else if (calomode== 1){
                parameterStore.get("LegoPlot", "nphicells").setI(64);
                parameterStore.get("LegoPlot", "netacells").setI(100);
            }
            else if (calomode== 2){
                parameterStore.get("LegoPlot", "nphicells").setI(32);
                parameterStore.get("LegoPlot", "netacells").setI(50);
            }
        }	
    }

} // class AGUI =============================================================
