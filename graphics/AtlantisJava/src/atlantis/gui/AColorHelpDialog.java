package atlantis.gui;

import java.awt.Color;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.JFrame;
import javax.swing.JTextField;

import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.APar;

/**
 * The help dialog displays the current color of objects
 * 
 * @author Mark Stockton
 */
public class AColorHelpDialog extends JFrame implements WindowListener
{
    private static JTextField[] textField;
    public static Color[] colorMap = AColorMap.getColors();
    private static AColorHelpDialog instance = null;

    private static APar parameterStore = APar.instance();
    private static AEventManager eventManager = AEventManager.instance();
    
    public static AColorHelpDialog getInstance()
    {
        if (AGlobals.isAtlantisHeadless()) {
            return null;
        }
        if (instance == null)
            instance = new AColorHelpDialog();
        return instance;
    }

    public void update()
    {
        if(!this.isVisible())
            return;
        getContentPane().removeAll();
        validate();
        
        String[] dataObjectsToDisplay={"RVx", "TRT", "SiCluster", "SiClusterRDO", "S3D", "PixelCluster", "PixelRDO", "TrigS3D",
                "MDT", "CSC", "RPC", "TGC",
                "EmTauROI", "JetROI", "MuonROI", "LVL1TriggerTower", "LVL1JetElement", 
                "STr", "SNP", "SMTr", "SVx"};
        String[] dataObjectsWithCollectionsToDisplay={"InDetTrack", "InDetSegment", "Cluster", "MuonTrack", 
                "MuonSegment", "Jet", "ETMis", "BJet","Electron","Muon","Photon","TauJet","CompositeParticle"};
        String[] geometryObjectsToDisplay={"PIXFill", "SILFill", "TRTFill", "SolFill", "ECALFill",
                "HCALFill", "RPCFill", "TGCFill", "MDTFill", "CSCFill", "BkgFill"};
        
        int numObjects = Math.max(dataObjectsWithCollectionsToDisplay.length,dataObjectsToDisplay.length);
        int numOfColumns = 4;
        getContentPane().setLayout(new GridLayout(numObjects+1,numOfColumns));
        textField = new JTextField[(numObjects+1)*4];
        AEvent event = eventManager.getCurrentEvent();
  
        //table column titles
        fillTextField(0, Color.white, Color.black, "       Geometry color");
        fillTextField(1, Color.white, Color.black, "       Color of objects without collections");
        fillTextField(2, Color.white, Color.black, "       Color of objects with collections");
        fillTextField(3, Color.white, Color.black, "       Object Collection");

        //loop over objects
        for(int j=0; j<numObjects; j++)
        {
            int i=4*j + 4;
            //display geometry color
            if(j<geometryObjectsToDisplay.length)
            {
            	String source = "Det";
            	if(geometryObjectsToDisplay[j].equals("BkgFill"))
            		source = "Color";
                Color backgroundColor=colorMap[parameterStore.get(source, geometryObjectsToDisplay[j]).getI()];
                //display screen name
                String text=parameterStore.get(source, geometryObjectsToDisplay[j]).getScreenName();
                fillTextField(i, backgroundColor, text);
            }else{
                fillTextField(i);//nothing to display
            }
            
            i++;
            //display object without collection color
            if(j<dataObjectsToDisplay.length){
                fillTextField(i,dataObjectsToDisplay[j]);
            }else{
                fillTextField(i);//nothing to display
            }

            i++;
            //display object with collection color
            if(j<dataObjectsWithCollectionsToDisplay.length){
                fillTextField(i,dataObjectsWithCollectionsToDisplay[j]);
            }else{
                fillTextField(i);//nothing to display
            }

            //display object collection
            i++;
            if(j<dataObjectsWithCollectionsToDisplay.length){
                //get collections
                String[] collections=event.getActiveCollectionNames(dataObjectsWithCollectionsToDisplay[j]);
                String text;
                //check is on/has collectons/not all selected
                if(!parameterStore.get("Data", dataObjectsWithCollectionsToDisplay[j]).getStatus() || 
                        collections==null || collections.length==0)
                    text = "n/a";
                else if(collections.length>1)
                    text = ">1 selected";
                else
                {
                    text = collections[0].substring(dataObjectsWithCollectionsToDisplay[j].length());
                }
                fillTextField(i, Color.white, Color.black, text);
                
            }else{
                 fillTextField(i);//nothing to display
            }
        }
        
        invalidate();
        pack();
    }
    
    /**
     * Fills the text field with chosen colours and text
     * foreground colour and text can be null to just fill background 
     * 
     * @param index - textfield index
     * @param background - background colour
     * @param foreground - foreground color
     * @param text - text to display
     */
    private void fillTextField(int index, Color background, Color foreground, String text)
    {
        if(textField[index]==null)
            textField[index]=new JTextField();
        textField[index].setEditable(false);
        textField[index].setBackground(background);
        if(foreground!=null)
            textField[index].setForeground(foreground);
        if(text!=null)
            textField[index].setText(text);
        getContentPane().add(textField[index]);
    }
    
    /**
     * Will fill a blank text field
     * 
     * @param index - textfield index
     */
    private void fillTextField(int index)
    {
        //nothing to display so set color to be the 
        //same as the outline color of the text boxes
        fillTextField(index, (new Color(184,207,229)), null, null);
    }

    /**
     * Automatically decides the forground text colour
     * depending on the background color
     * 
     * @param index - textfield index
     * @param background - background colour
     * @param text - text to display
     */
    private void fillTextField(int index, Color background, String text)
    {
        //find out if too dark for black text
        if( background.getBlue()==background.getRed()
                && background.getBlue()==background.getGreen()
                && background.getBlue()<=128)
            fillTextField(index, background, Color.white, text);
        else
            fillTextField(index, background, Color.black, text);
    }
    
    /**
     * Get information from object for background
     * colour and objedct name + status gives the text
     * 
     * @param index - textfield index
     * @param object - name of object
     */
    private void fillTextField(int index, String object)
    {
        //if color by constant or has no colour option (e.g. vertex)
        if(parameterStore.get(object, "ColorFunction")==null 
                || parameterStore.get(object, "ColorFunction").getI()==0)
        {
            Color backgroundColor=colorMap[parameterStore.get(object, "Constant").getI()];
            String text = object;
            if(!parameterStore.get("Data", object).getStatus())
                text += " - data type not active";
            fillTextField(index, backgroundColor, text);
        }
        else
        {
            String text = object;
            if(parameterStore.get("Data", object).getStatus())
                text += " is not being colored constant";
            else
                text += " - data type not active";
            fillTextField(index, Color.white, Color.black, text);
        }
    }
        
    private AColorHelpDialog()
    {
        this.setTitle("Current window Geometry and Object colors and Object Collection");
        AIcon.setIconImage(this);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        this.update();
        this.setResizable(false);
        addWindowListener(this);

        // set the initial location
        int screenWidth = Math.round((float) Toolkit.getDefaultToolkit().getScreenSize().getWidth());
        int screenHeight = Math.round((float) Toolkit.getDefaultToolkit().getScreenSize().getHeight());
        this.setLocation((int) Math.max(0, 0.1*screenWidth), (int) Math.max(0, 0.1*screenHeight));
    }

    public void dispose()
    {
        instance = null;
        super.dispose();
    }
    
    public void windowOpened(WindowEvent e)
    {
        this.update();
    }
    
    public void windowClosing(WindowEvent e)
    {}
    
    public void windowClosed(WindowEvent e)
    {}
    
    public void windowIconified(WindowEvent e)
    {}
    
    public void windowDeiconified(WindowEvent e)
    {}
    
    public void windowActivated(WindowEvent e)
    {}
    
    public void windowDeactivated(WindowEvent e)
    {}
    
}