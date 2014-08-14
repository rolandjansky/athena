package atlantis.canvas;


import java.util.Hashtable;
import java.util.Enumeration;
import java.awt.Rectangle;
import java.awt.Dimension;
import org.w3c.dom.*;


/**
 * This class is not an Layout Manager! It contains the possible window
 * configuration - i.e. the possible subdivision of the canvas area in windows.
 * Layouts are identified by names such as "SQUARE","FULL_SCREEN".
 * Windows are identified by single characters such as "W","1", etc.
 * Windows are furthermore grouped in pages such that they do not overlap.
 * Pages are identified by the sum of the windows they contain, such as "Full","Vert"
 * This class is used mainly by ACanvas and AWindowControl.
 */

public class ALayout {
    //The name of the layout
    private String name;
    //A table of the window constraints (i.e. corners) for all windows
    private Hashtable windowConstraints;
    //A table of the pages
    private Hashtable pages;
    //The default top window for this layout
    private String startupWindow;
    //The list of window names that by default are
    //on the top in this layout
    private String[] startupSequence;
    
    
    /**
     * Default constructor
     * 
     * @param node XML node with the configuration
     */
    public ALayout(Node node) {
        //Create new hashtables for window corners and pages
        windowConstraints=new Hashtable();
        pages=new Hashtable();
        
        // reading the layout attributes
        NamedNodeMap attributes=node.getAttributes();
        name=attributes.getNamedItem("name").getNodeValue();
        startupWindow=attributes.getNamedItem("startupWindow").getNodeValue();
        startupSequence=decompose(attributes.getNamedItem("startup").getNodeValue());
        
        // reading all the content of Layout node
        // - i.e pages and window settings
        NodeList childrens=node.getChildNodes();    
        for(int i=0; i<childrens.getLength(); i++) {
            //get the subnode
            Node child=childrens.item(i);
            //read in window or page information
            if(child.getNodeType()==Node.ELEMENT_NODE) {
                //check wether it is a window or page configuration
                String nodeName=child.getNodeName();
                if(nodeName.equals("Window"))
                    readWindow(child);
                else if(nodeName.equals("Page"))
                    readPage(child);
            }
        }
    }
    
    /**
     * Read the window configuration (name/size/position) from an XML node
     * 
     * @param node XML node with the configuration
     */
    private void readWindow(Node child) {
        //Get the nodes attributes
        NamedNodeMap attributes=child.getAttributes();
        //retrieve window information
        String name=attributes.getNamedItem("name").getNodeValue();
        int hPos=Integer.parseInt(attributes.getNamedItem("hPos").getNodeValue());
        int vPos=Integer.parseInt(attributes.getNamedItem("vPos").getNodeValue());
        int width=Integer.parseInt(attributes.getNamedItem("width").getNodeValue());
        int height=Integer.parseInt(attributes.getNamedItem("height").getNodeValue());
        //Check if this is a valid window name
        if(!ACanvas.getCanvas().isValidWindowName(name))
            throw new Error("Undefined window name: "+name);
        //Check if this window has been defined before
        if(windowConstraints.containsKey(name))
            throw new Error("Multiple usage of window name: "+name);
        //Validate window size and position
        if((hPos<0)||(vPos<0)||(width<0)||(height<0))
            throw new Error("Wrong window constraints");
        //Add window constraints to the list
        windowConstraints.put(name, new Rectangle(hPos, vPos, width, height));
    }
    
    /**
     * Read the page configuration (windows it contains) from the XML node
     */
    private void readPage(Node child) {
        //get the attributes
        NamedNodeMap attributes=child.getAttributes();
        //retrieve page configuration
        String name=attributes.getNamedItem("name").getNodeValue();
        String content=attributes.getNamedItem("content").getNodeValue();
        
        //Check if this page exists before
        if(pages.containsKey(name))
            throw new Error("Redefinition of page: "+name);
        //Split the list of windows in single window names
        //('S789' -> 'S','7','8','9')
        String[] wContent=decompose(content);
        
        //Check that each of this is a valid window
        for(int i=0; i<wContent.length; i++)
            if(!ACanvas.getCanvas().isValidWindowName(wContent[i]))
                throw new Error("Unknown window name: "+wContent[i]);
        //Add this page
        pages.put(name, wContent);
    }
    
    /** 
     * Break up a string of window names into a list of single window names
     *
     * @param s the string to break up
     */
    private String[] decompose(String s) {
        String[] strings=new String[s.length()];
        char[] characters=s.toCharArray();
        
        for(int i=0; i<characters.length; i++)
            strings[i]=new String(new char[] {characters[i]});
        
        return strings;
    }
    
    /**
     * Return the layout name
     */
    public String getName() {
        return name;
    }
    
    /**
     * Return the window selected on startup
     */
    public String getStartupWindow() {
        return startupWindow;
    }

    /**
     * Return the list of window names that are on top by default
     */
    public String[] getStartupSequence() {
        return startupSequence;
    }
    
    /**
     * Check if window is part of this layout
     */
    public boolean hasWindow(String wName) {
        return windowConstraints.containsKey(wName);
    }
    
    /**
     * Return window names in this layout
     */
    public String[] getWindowNames() {
        // Loop over the window constraints and retrieve
        // window names.
        Enumeration wn=windowConstraints.keys();
        int size=windowConstraints.size();
        String[] names=new String[size];
        //Add them all to the array
        for(int i=0; i<size; i++)
            names[i]=(String)wn.nextElement();
        
        return names;
    }
    
    /**
     * Return the constraints (i.e. position and size)
     * for a particular window
     *
     * @param wName the name of the window
     */
    public Rectangle getWindowConstraints(String wName) {
        return(Rectangle)windowConstraints.get(wName);
    }
    
    /**
     * Calculate the size of the layout from all its subwindows
     */
    public Dimension getSize() {
       
        Enumeration myenum=windowConstraints.elements();
        int w=0, h=0;
        //Loop over all windows
        while(myenum.hasMoreElements()) {
            Rectangle r=(Rectangle)myenum.nextElement();
            //Check if they exceed the current size
            if(r.x+r.width>w)
                w=r.x+r.width;
            if(r.y+r.height>h)
                h=r.y+r.height;
        }
        
        return new Dimension(w, h);
    }
    
    /**
     * Return a list of the page names
     */
    public String[] getPageNames() {
        Enumeration myenum=pages.keys();
        int size=pages.size();
        String[] names=new String[size];
        
        for(int i=0; i<size; i++)
            names[i]=(String)myenum.nextElement();
        
        return names;
    }
    
    /**
     * Return a list of the windows that a certain page consists of
     */
    public String[] getPageContent(String pageName) {
        return(String[])pages.get(pageName);
    }
    
}
