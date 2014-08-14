package atlantis.gui;

import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.Frame;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.util.List;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JScrollPane;

import com.Ostermiller.util.CSVParser;

import atlantis.data.ACalorimeterData;
import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.data.ALVL1ResultData;
import atlantis.event.ANewEventListener;
import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.graphics.layout.AFlowLayout;
import atlantis.data.ATriggerInfoData;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;

/**
 * The dialog which is used to display the current event properties.
 */
public class AEventPropertiesDialog extends JFrame implements ANewEventListener
{
    private static ALogger logger = ALogger.getLogger(AEventPropertiesDialog.class);
    private static AEventManager eventManager = AEventManager.instance();
    
    private JTable table, propertiesTable, level1Table, level2Table, 
                   eventFilterTable, trigInfoItemsTable, trigInfoStreamTable,
                   userSelectedTable, mbtsTable;
    private JPanel buttonsPanel;
    private JTabbedPane tabbedPane;
    private JButton okButton;
    private JButton refreshButton;
    private static AEventPropertiesDialog instance = null;
    protected static String[] USER_ITEMS = null;
    private static final String LOOKUP_FILENAME = "epusti.csv";
    private static final AGlobals globals = AGlobals.instance();
    private static final String LOOKUP_FILE =
    		globals.getHomeDirectory() + "configuration" + 
        		System.getProperty("file.separator") +
        		LOOKUP_FILENAME;

    public static boolean isInUse()
    {
        if(instance ==null || !instance.isVisible())
            return false;
        else
            return true;
    }
    
    public static AEventPropertiesDialog getInstance()
    {
        if (instance == null)
            instance = new AEventPropertiesDialog();
        return instance;
    }

    public AEventPropertiesDialog()
    {
    	Frame gui = globals.getGuiFrame();
    	eventManager.addNewEventListener(this);
        this.setTitle("Event Properties Dialog");
        AIcon.setIconImage(this);
        // setResizable(false);
        
        // From Java 1.5 we can tell the window to be always on top
        this.setAlwaysOnTop(true);
        
        // check whether there is any event data are present
        if (eventManager.getCurrentEvent() == null)
        {
            JOptionPane.showMessageDialog(gui,
                    "Current event is null\n" + "(no event data currently present)",
                    "Event null", JOptionPane.WARNING_MESSAGE);
            return;
        }
        
        try
        {
            readFromLookupFile();
        }
        catch(AAtlantisException ex)
        {
            logger.warn(": reading " + LOOKUP_FILE +
                    " failed, user trigger items will not " +
                    "be available, reason: " + ex.getMessage(), ex);
        }
        
        fillTabbedPane(0);
        addButtonPane();
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);        
        pack();

        // set the initial location
        int guiWidth = gui.getWidth();
        int guiHeight = gui.getHeight();
        int guiX = gui.getX();
        int dialogWidth = (int) this.getPreferredSize().getWidth();
        int dialogHeight = (int) this.getPreferredSize().getHeight();
        int screenWidth = Math.round((float) Toolkit.getDefaultToolkit()
                .getScreenSize().getWidth());
        if (guiX + guiWidth + (dialogWidth - guiWidth) / 2 > screenWidth)
            this.setLocation(Math.max(0, screenWidth - dialogWidth), Math.max(
                    0, (guiHeight - dialogHeight) / 3));
        else
            this.setLocation(Math.max(0, guiX + (guiWidth - dialogWidth) / 2),
                    Math.max(0, (guiHeight - dialogHeight) / 3));

        setVisible(true);
    }

    private void readFromLookupFile() throws AAtlantisException
    {
        if(USER_ITEMS==null)
        {
            try
            {
                InputStream is = AUtilities.getFileAsStream(LOOKUP_FILE);
                CSVParser parser = new CSVParser(is);
                parser.setCommentStart("#");
                String valueArray = parser.nextValue(); // shall now contain all values
                CSVParser parserArray = new CSVParser(new StringReader(valueArray));
                String[][] s = parserArray.getAllValues();
                if(s!=null)
                {
                    USER_ITEMS = s[0];
                }
            }
            catch (IOException e)
            {
                   throw new AAtlantisException("exception while reading file: " +
                            LOOKUP_FILE);
            }
            catch (AAtlantisException e)
            {
                throw e;
            }
        }
    }

    
    public void dispose()
    {
        instance = null;
        super.dispose();
    }

    public void updatePane()
    {
    	
        JTabbedPane currentTabbedPane = (JTabbedPane) getContentPane().getComponent(0);
        int currentIndex = currentTabbedPane.getSelectedIndex();
        getContentPane().removeAll();
        validate();
        
        fillTabbedPane(currentIndex);
        addButtonPane();
        
        pack();
    }
    
    private void addButtonPane()
    {
        okButton = new JButton("OK");
        okButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                dispose();
            }
        });
        
        // Add button to refresh data after cuts have been applied
        refreshButton = new JButton("Refresh Cuts");
        refreshButton.setToolTipText("Display data passing cuts for active window");
        refreshButton.addActionListener(new ActionListener() {
        	public void actionPerformed(ActionEvent e)
        	{
        		updatePane();
        	}
        });

        buttonsPanel = new JPanel(new AFlowLayout(10, 5));
        buttonsPanel.add(okButton);
        buttonsPanel.add(refreshButton);
        getContentPane().add(buttonsPanel, BorderLayout.SOUTH);
        
      
        
    }
    
    /** fills the tabbed pane with the event information
     *  @param index the currently selected index 
     */
    private void fillTabbedPane(int index)
    {
        fillTablePane();
        AEvent event = eventManager.getCurrentEvent();
        
        //now need to loop over the lvl1result collections
        Vector<String> keys = event.getCollections().get("LVL1Result");
        ALVL1ResultData lvl1ResultData[]=null;
        if(keys!=null){
            lvl1ResultData = new ALVL1ResultData[keys.size()];
            for(int i=0; i<keys.size();i++)
            {
                String colLvl1= "LVL1Result" + (String)keys.elementAt(i);
                lvl1ResultData[i] = (ALVL1ResultData) event.get(colLvl1);
            }
        }
        
        ATriggerInfoData triggerInfoData = event.getTriggerInfoData();
        fillPropertiesPane(lvl1ResultData, triggerInfoData);
        String[][] L1_items = fillLevel1Pane(lvl1ResultData);
        String[][] L2_items = fillLevel2Pane(lvl1ResultData);
        String[][] EF_items = fillEventFilterPane(lvl1ResultData);
        fillTrigInfoItemsPane(triggerInfoData);
        fillTrigInfoStreamPane(triggerInfoData);
        fillUserSelectedPane(L1_items, L2_items, EF_items);
        fillmbtsPane();
        
        tabbedPane = new JTabbedPane();
        tabbedPane.addTab("Number of Hits", new JScrollPane(table));
        tabbedPane.addTab("Properties", new JScrollPane(propertiesTable));
        tabbedPane.addTab("Level 1 Trigger Items", new JScrollPane(level1Table));
        tabbedPane.addTab("Level 2 Trigger Items", new JScrollPane(level2Table));
        tabbedPane.addTab("Event Filter Trigger Items", new JScrollPane(eventFilterTable));
        tabbedPane.addTab("Trigger Info Items", new JScrollPane(trigInfoItemsTable));
        tabbedPane.addTab("Trigger Info Stream Tag", new JScrollPane(trigInfoStreamTable));
        tabbedPane.addTab("User Selected Trigger Items", new JScrollPane(userSelectedTable));
        tabbedPane.addTab("All MBTS cell details", new JScrollPane(mbtsTable));
        tabbedPane.setSelectedIndex(index);
        
        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(tabbedPane, BorderLayout.CENTER);
    }
    
    private void fillTablePane()
    {
    	// For total data and data passing cuts, use .getInfoDraw()
    	// for total event data, use getInfo()
        table = new JTable(eventManager.getCurrentEvent().getInfoDraw(),
                new String[] { "detector / datatype", "data items", "passing cuts" }) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        table.setRowHeight(20);
        table.setFont(new Font("Monospaced", Font.PLAIN, 16));
        table.getColumnModel().getColumn(0).setPreferredWidth(350);
        table.getColumnModel().getColumn(1).setPreferredWidth(100);
        table.getColumnModel().getColumn(2).setPreferredWidth(100);
    }

    private void fillPropertiesPane(ALVL1ResultData lvl1ResultData[], ATriggerInfoData triggerInfoData)
    {
        double et = 0;
        List<ACalorimeterData> calo = eventManager.getCurrentEvent().getCalorimeters();

        for (int i = 0; i < calo.size(); i++)
            et += calo.get(i).getTotalTransverseEnergy();

        String[][] properties;
        
        if (lvl1ResultData == null)
        {
            if(triggerInfoData==null)
            {
                properties = new String[2][2]; 
                properties[1][0] = "Trigger Decision";
                properties[1][1] = "N/A";
            }
            else
            {
                properties = new String[7][2]; 
                properties[1][0] = "Trigger Decision";
                properties[1][1] = "N/A";
                properties[2][0] = "Trigger Info: Level 1 EtMiss";
                properties[2][1] = String.valueOf(triggerInfoData.getEnergyEtMiss(0));
                properties[3][0] = "Trigger Info: Level 1 SumEt";
                properties[3][1] = String.valueOf(triggerInfoData.getEnergyEtMiss(0));
                properties[4][0] = "Trigger Info: ExtL1ID";
                properties[4][1] = triggerInfoData.getTrigInfoExtL1ID(0);
                properties[5][0] = "Trigger Info: Lvl1Type";
                properties[5][1] = triggerInfoData.getTrigInfoLvl1Type(0);
                properties[6][0] = "Trigger Info: Status";
                properties[6][1] = triggerInfoData.getTrigInfoStatus(0);
                
            }
        }
        else
        {
            properties = new String[5*lvl1ResultData.length+6][2];
            
            //output the trigger info:
            properties[1][0] = "Trigger Info: Level 1 EtMiss";
            properties[2][0] = "Trigger Info: Level 1 SumEt";
            properties[3][0] = "Trigger Info: ExtL1ID";
            properties[4][0] = "Trigger Info: Lvl1Type";
            properties[5][0] = "Trigger Info: Status";

            if(triggerInfoData!=null)
            {
                properties[1][1] = String.valueOf(triggerInfoData.getEnergyEtMiss(0));
                properties[2][1] = String.valueOf(triggerInfoData.getEnergySumEt(0));
                properties[3][1] = triggerInfoData.getTrigInfoExtL1ID(0);
                properties[4][1] = triggerInfoData.getTrigInfoLvl1Type(0);
                properties[5][1] = triggerInfoData.getTrigInfoStatus(0);
            }else{
                properties[1][1] = "N/A";
                properties[2][1] = "N/A";
                properties[3][1] = "N/A";
                properties[4][1] = "N/A";
                properties[5][1] = "N/A";
            }
            
            for(int j=0; j<lvl1ResultData.length;j++){
                int ref=j*5;//reference to allow >1 collection in the list
                String name=lvl1ResultData[j].getStoreGateKey();
            
                properties[6+ref][0] = name + ": Level 1 result";
                properties[7+ref][0] = name + ": Level 2 result";
                properties[8+ref][0] = name + ": Event Filter result";
                properties[9+ref][0] = name + ": Level 1 EtMiss";
                properties[10+ref][0] = name + ": Level 1 SumEt";
    
                int[] temp=new int[3];
                temp[0]=lvl1ResultData[j].getPassedL1(0);
                temp[1]=lvl1ResultData[j].getPassedL2(0);
                temp[2]=lvl1ResultData[j].getPassedEF(0);
                for(int i=0;i<3;i++)
                {
                    switch(temp[i])
                    {
                        case -1:
                            properties[i+6+ref][1]="N/C";
                            break;
                        case 0:
                            properties[i+6+ref][1]="failed";
                            break;
                        case 1:
                            properties[i+6+ref][1]="passed";
                            break;
                        default:
                            properties[i+6+ref][1]="N/A";
                    }
                }
                float LVL1EtMiss=lvl1ResultData[j].getEnergyEtMiss(0);
                float LVL1SumEt=lvl1ResultData[j].getEnergySumEt(0);
                if(LVL1EtMiss>0) 
                    properties[9+ref][1] = String.valueOf(LVL1EtMiss);
                else 
                    properties[9+ref][1] = "N/A";
                if(LVL1SumEt>0) 
                    properties[10+ref][1] = String.valueOf(LVL1SumEt);
                else 
                    properties[10+ref][1] = "N/A";
            }
        }
        
        //first item always Transv. Energy
        properties[0][0] = "Transv. Energy (GeV)"; 
        properties[0][1] = String.format("%.2f",et);
        
        propertiesTable = new JTable(properties, new String[] { "Name", "Value" }) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        propertiesTable.setRowHeight(20);
        propertiesTable.setFont(new Font("Monospaced", Font.PLAIN, 16));
        propertiesTable.getColumnModel().getColumn(0).setPreferredWidth(350);
        propertiesTable.getColumnModel().getColumn(1).setPreferredWidth(100);
    }

    private String[][] fillLevel1Pane(ALVL1ResultData lvl1ResultData[])
    {
        String[][] properties;
        
        if (lvl1ResultData == null)
        {
            properties = new String[1][2]; 
            properties[0][0] = "N/A"; 
            properties[0][1] = "N/A";
        }
        else
        {
            String[][] items=new String[lvl1ResultData.length][];
            String[][] prescales=new String[lvl1ResultData.length][];
            int numItems=0;
            for(int j=0; j<lvl1ResultData.length;j++){
                items[j]=lvl1ResultData[j].getCtpItemListSplit(0,false);
                prescales[j]=lvl1ResultData[j].getPrescaleListL1Split(0);
                numItems+=items[j].length;
            }
            properties = new String[numItems][2]; 
            numItems=0;
            for(int j=0; j<lvl1ResultData.length;j++){
                String name=lvl1ResultData[j].getStoreGateKey();
                for(int i=0; i<items[j].length; i++)
                {
                    properties[numItems][0]= name + ": " + items[j][i];
                    properties[numItems][1]=(prescales[j] != null && i<prescales[j].length) ? prescales[j][i]: "N/A";
                    numItems++;
                }
            }
        }
        level1Table = new JTable(properties, new String[] { "Item passed", "Prescale"}) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        level1Table.setRowHeight(20);
        level1Table.setFont(new Font("Monospaced", Font.PLAIN, 16));
        level1Table.getColumnModel().getColumn(0).setPreferredWidth(350);
        level1Table.getColumnModel().getColumn(1).setPreferredWidth(100);
        
        return properties;
    }

    private String[][] fillLevel2Pane(ALVL1ResultData lvl1ResultData[])
    {
        String[][] properties;
        
        if (lvl1ResultData == null)
        {
            properties = new String[1][2]; 
            properties[0][0] = "N/A"; 
            properties[0][1] = "N/A";
        }
        else
        {
            String[][] items=new String[lvl1ResultData.length][];
            String[][] prescales=new String[lvl1ResultData.length][];
            int numItems=0;
            for(int j=0; j<lvl1ResultData.length;j++){
                items[j]=lvl1ResultData[j].getitemListL2Split(0,false);
                prescales[j]=lvl1ResultData[j].getPrescaleListL2Split(0);
                numItems+=items[j].length;
            }
            properties = new String[numItems][2]; 
            numItems=0;
            for(int j=0; j<lvl1ResultData.length;j++){
                String name=lvl1ResultData[j].getStoreGateKey();
                for(int i=0; i<items[j].length; i++)
                {
                    properties[numItems][0]= name + ": " + items[j][i];
                    properties[numItems][1]=(prescales[j] != null && i<prescales[j].length) ? prescales[j][i]: "N/A";
                    numItems++;
                }
            }
        }
        level2Table = new JTable(properties, new String[] { "Item passed", "Prescale" }) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        level2Table.setRowHeight(20);
        level2Table.setFont(new Font("Monospaced", Font.PLAIN, 16));
        level2Table.getColumnModel().getColumn(0).setPreferredWidth(350);
        level2Table.getColumnModel().getColumn(1).setPreferredWidth(100);
        
        return properties;
    }

    private String[][] fillEventFilterPane(ALVL1ResultData lvl1ResultData[])
    {
        String[][] properties;
        
        if (lvl1ResultData == null)
        {
            properties = new String[1][2]; 
            properties[0][0] = "N/A";
            properties[0][1] = "N/A";
        }
        else
        {
            String[][] items=new String[lvl1ResultData.length][];
            String[][] prescales=new String[lvl1ResultData.length][];
            int numItems=0;
            for(int j=0; j<lvl1ResultData.length;j++){
                items[j]=lvl1ResultData[j].getitemListEFSplit(0,false);
                prescales[j]=lvl1ResultData[j].getPrescaleListEFSplit(0);
                numItems+=items[j].length;
            }
            properties = new String[numItems][2]; 
            numItems=0;
            for(int j=0; j<lvl1ResultData.length;j++){
                String name=lvl1ResultData[j].getStoreGateKey();
                for(int i=0; i<items[j].length; i++)
                {
                    properties[numItems][0]= name + ": " + items[j][i];
                    properties[numItems][1]=(prescales[j] != null && i<prescales[j].length) ? prescales[j][i]: "N/A";
                    numItems++;
                }
            }
        }
        eventFilterTable = new JTable(properties, new String[] { "Item passed", "Prescale" }) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        eventFilterTable.setDefaultEditor(String.class, null);
        eventFilterTable.setRowHeight(20);
        eventFilterTable.setFont(new Font("Monospaced", Font.PLAIN, 16));
        eventFilterTable.getColumnModel().getColumn(0).setPreferredWidth(350);
        eventFilterTable.getColumnModel().getColumn(1).setPreferredWidth(100);
        
        return properties;
    }

    private void fillTrigInfoItemsPane(ATriggerInfoData triggerInfoData)
    {
        String[][] properties;
        
        if (triggerInfoData == null)
        {
            properties = new String[1][3]; 
            properties[0][0] = "N/A";
            properties[0][1] = "N/A";
            properties[0][2] = "N/A";
        }
        else
        {
            String[] itemsL1=triggerInfoData.getTrigInfoL1SplitHex(0);
            String[] itemsL2=triggerInfoData.getTrigInfoL2SplitHex(0);
            String[] itemsEF=triggerInfoData.getTrigInfoEFSplitHex(0);
            //find list with largest amount of items
            int items = Math.max(Math.max(itemsL1.length, itemsL2.length),itemsEF.length);
            properties = new String[items][3]; 
            for(int i=0; i<items; i++)
            {
                properties[i][0]= (i<itemsL1.length) ? itemsL1[i] : "";
                properties[i][1]= (i<itemsL2.length) ? itemsL2[i] : "";
                properties[i][2]= (i<itemsEF.length) ? itemsEF[i] : "";
            }
        }
        trigInfoItemsTable = new JTable(properties, new String[] { "L1", "L2", "EF" }) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        trigInfoItemsTable.setRowHeight(20);
        trigInfoItemsTable.setFont(new Font("Monospaced", Font.PLAIN, 16));
        trigInfoItemsTable.getColumnModel().getColumn(0).setPreferredWidth(100);
        trigInfoItemsTable.getColumnModel().getColumn(1).setPreferredWidth(100);
        trigInfoItemsTable.getColumnModel().getColumn(1).setPreferredWidth(100);

    }

    private void fillTrigInfoStreamPane(ATriggerInfoData triggerInfoData)
    {
        String[][] properties;
        
        if (triggerInfoData == null)
        {
            properties = new String[1][1]; 
            properties[0][0] = "N/A";
        }
        else
        {
            String[] items=triggerInfoData.getTrigInfoStreamTagSplit(0);
            properties = new String[items.length][1]; 
            for(int i=0; i<items.length; i++)
            {
                properties[i][0]=items[i];
            }
        }
        trigInfoStreamTable = new JTable(properties, new String[] { "Stream Tag"}) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        trigInfoStreamTable.setRowHeight(20);
        trigInfoStreamTable.setFont(new Font("Monospaced", Font.PLAIN, 16));
        trigInfoStreamTable.getColumnModel().getColumn(0).setPreferredWidth(450);
    }

    private void fillUserSelectedPane(String[][] L1_items, String[][] L2_items, String[][] EF_items)
    {
        String[][] properties;
        if (USER_ITEMS==null)
        {
            properties = new String[1][2]; 
            properties[0][0] = "Please add items to: " + LOOKUP_FILENAME;
            properties[0][1] = "N/A";
        }
        else
        {
            Vector<String> v_c1 = new Vector<String>();//column 1 - item
            Vector<String> v_c2 = new Vector<String>();//column 2 - prescales
            Vector<String> v_u = new Vector<String>();//user items
            for(int i=0; i<USER_ITEMS.length; i++)
            {
                v_u.add(USER_ITEMS[i]);
            }
            //loop over L1 items
            for(int j=0; j<L1_items.length; j++)
            {
                for(int i=0; i<v_u.size(); i++)
                {
                    if(v_u.elementAt(i).equals(L1_items[j][0]))
                    {
                        v_c1.add(L1_items[j][0]);
                        v_c2.add(L1_items[j][1]);
                        v_u.remove(i);
                    }                    
                }
            }
            //if still have user items left loop over L2
            if(v_u.size()>0)
            {
                for(int j=0; j<L2_items.length; j++)
                {
                    for(int i=0; i<v_u.size(); i++)
                    {
                        if(v_u.elementAt(i).equals(L2_items[j][0]))
                        {
                            v_c1.add(L2_items[j][0]);
                            v_c2.add(L2_items[j][1]);
                            v_u.remove(i);
                        }                    
                    }
                }
            }
            //if still have user items left loop over EF
            if(v_u.size()>0)
            {
                for(int j=0; j<EF_items.length; j++)
                {
                    for(int i=0; i<v_u.size(); i++)
                    {
                        if(v_u.elementAt(i).equals(EF_items[j][0]))
                        {
                            v_c1.add(EF_items[j][0]);
                            v_c2.add(EF_items[j][1]);
                            v_u.remove(i);
                        }                    
                    }
                }
            }            
            //now store items in String[][]
            properties = new String[v_c1.size()][2];
            for(int i=0; i<v_c1.size(); i++)
            {
                properties[i][0]=v_c1.elementAt(i).toString();
                properties[i][1]=v_c2.elementAt(i).toString();
            }
        }
        userSelectedTable = new JTable(properties, new String[] { "Item passed", "Prescale" }) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        userSelectedTable.setRowHeight(20);
        userSelectedTable.setFont(new Font("Monospaced", Font.PLAIN, 16));
        userSelectedTable.getColumnModel().getColumn(0).setPreferredWidth(350);
        userSelectedTable.getColumnModel().getColumn(1).setPreferredWidth(100);
    }
    
    private void fillmbtsPane()
    {
        ACalorimeterData calorimeter=null;
        List<ACalorimeterData> detectors = eventManager.getCurrentEvent().getCalorimeters();
        for (int det = 0; det < detectors.size(); det++)
        {
            ACalorimeterData testcalorimeter = detectors.get(det);
            if (!testcalorimeter.getName().equals("MBTS")) continue;
            if(testcalorimeter.getNumData()>0) calorimeter=testcalorimeter;
        }
        
            
        String[][] properties;
        if (calorimeter==null)
        {
            properties = new String[1][5]; 
            properties[0][0] = "N/A";
            properties[0][1] = "N/A";
            properties[0][2] = "N/A";
            properties[0][3] = "N/A";
            properties[0][4] = "N/A";
        }
        else
        {
            properties = new String[calorimeter.getNumData()][5];
            //AMBTSData mbtsdata = calorimeter.eta[i];
            for (int i = 0; i < calorimeter.getNumData(); ++i)
            {
                properties[i][0] = "" + i;
                properties[i][1] = "" + String.format("%.3f",(calorimeter.getET(i)));
                properties[i][2] = "" + String.format("%.1f",calorimeter.getEta(i));
                properties[i][3] = "" + String.format("%.1f",calorimeter.getPhi(i));
                properties[i][4] = "" + calorimeter.getSampling(i);
            }
        }
        mbtsTable = new JTable(properties, new String[] { "Index", "Energy (MeV)", "Type", "Module", "Channel" }) {
            public boolean isCellEditable(int row, int column) { return false; }
        };
        mbtsTable.setRowHeight(20);
        mbtsTable.setFont(new Font("Monospaced", Font.PLAIN, 16));
        mbtsTable.getColumnModel().getColumn(0).setPreferredWidth(90);
        mbtsTable.getColumnModel().getColumn(1).setPreferredWidth(90);
        mbtsTable.getColumnModel().getColumn(2).setPreferredWidth(90);
        mbtsTable.getColumnModel().getColumn(3).setPreferredWidth(90);
        mbtsTable.getColumnModel().getColumn(4).setPreferredWidth(90);
    }

    /**
     * Implementing NewEvent listener
     * @param event the new event
     */
    public void newEvent(AEvent event) {
      if (isInUse()) updatePane();
    }
    
    
    
    
}
