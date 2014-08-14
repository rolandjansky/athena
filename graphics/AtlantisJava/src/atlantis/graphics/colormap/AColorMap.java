package atlantis.graphics.colormap;

import java.io.File;
import java.io.InputStream;
import java.awt.Color;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.NamedNodeMap;

import atlantis.globals.AGlobals;
import atlantis.output.AExceptionHandler;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.APar;
import atlantis.utils.AUtilities;
import atlantis.utils.ALogger;
import atlantis.utils.xml.AXMLErrorHandler;


public final class AColorMap
{
	private static ALogger logger = ALogger.getLogger(AColorMap.class);
	
    private static final boolean VALIDATION = true;
    final public static int NO_COLOR = -1;
    final public static int INVISIBLE = -2;
    static final public int BG = 8;   //background
    static final public int WH = 0;   //white
    static final public int GY = 14;  //grey
    static final public int RD = 20;  //red
    static final public int GN = 21;  //green
    static final public int BL = 22;  //blue
    static final public int YE = 23;  //yellow
    static final public int MA = 24;  //magenta
    static final public int CY = 25;  //cyan
    static final public int BK = 26;  //black
    static final public int OR = 27;  //orange
    static final public int CB = 28;  //cornflower blue
    private static final String[] mapNames = {"Default(1)", "Default(2)", "M4+M5", "GrayDet", 
                                             "Original", "Gray", "B/W", "HitCol", "GrayHitCol"};
    private static final String[] mapNamesPS = {"default1","default2","m4m5", "graydet", 
                                             "original", "gray",  "bw", "hitcol", "grayhitcol"};
    private static MappedColor[][] maps;
    private static int numColors;
    private static int numMaps;

    // Color map indices.
    public static final int COLOR_MAP_DEFAULT1 = 0;
    public static final int COLOR_MAP_DEFAULT2 = 1;
    public static final int COLOR_MAP_M4M5 = 2;
    public static final int COLOR_MAP_GRAYDET = 3;
    public static final int COLOR_MAP_ORIGINAL = 4;
    public static final int COLOR_MAP_GRAY = 5;
    public static final int COLOR_MAP_BW = 6;
    public static final int COLOR_MAP_HITCOL = 7;
    public static final int COLOR_MAP_GRAY_HITCOL = 8;

    //color map used if wrong value in config file 
    static int currentMap = COLOR_MAP_DEFAULT1;


    public static void readColorMap(String name)
    {
        String fileName = null;
        String FILE_SEPAR = System.getProperty("file.separator");

        if(name != null && new File(name).canRead())
        {
            // color map file specified as command line argument
            fileName = name;
        }
        else
        {
            if(name != null)
            {
                logger.warn("Can't read color map file: " + name);
            }
            // color map file wasn't specified as command line argument
            fileName = ".Atlantis-colormap.xml"; // default name
            String fileNameFull = System.getProperty("user.home") + FILE_SEPAR + fileName;
            if(new File(fileNameFull).canRead())
            {
                // use existing user's color map file
                fileName = fileNameFull;
            }
            else
            {
                // use default color map file from Atlantis distribution
                fileName = AGlobals.instance().getHomeDirectory() + "configuration" +
                           FILE_SEPAR + "colormap.xml";
            }
        }

        logger.info("Using color map file: " + fileName);
        try
        {
        	InputStream isColorMap = AUtilities.getFileAsStream(fileName);
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            factory.setValidating(VALIDATION);
            DocumentBuilder parser = factory.newDocumentBuilder();
            parser.setErrorHandler(new AXMLErrorHandler());
            Node colormap = parser.parse(isColorMap).getDocumentElement();
            NamedNodeMap cm = colormap.getAttributes();
            numMaps = Integer.parseInt(cm.getNamedItem("numMaps").getNodeValue().
                                       trim());
            numColors = Integer.parseInt(cm.getNamedItem("numColors").
                                         getNodeValue().trim());
            maps = new MappedColor[numMaps][numColors];
            NodeList list = colormap.getChildNodes();
            int count = list.getLength();
            for(int i = 0; i < count; i++)
            {
                Node mapping = list.item(i);
                if(mapping.getNodeType() == Node.ELEMENT_NODE)
                {
                    NamedNodeMap atts = mapping.getAttributes();
                    try
                    {
                        int map = Integer.parseInt(atts.getNamedItem("map").
                            getNodeValue().trim());
                        int index = Integer.parseInt(atts.getNamedItem("index").
                            getNodeValue().trim());
                        int r = Integer.parseInt(atts.getNamedItem("r").
                                                 getNodeValue().trim());
                        int g = Integer.parseInt(atts.getNamedItem("g").
                                                 getNodeValue().trim());
                        int b = Integer.parseInt(atts.getNamedItem("b").
                                                 getNodeValue().trim());
                        if(map < maps.length)
                        {
                            if(index < maps[map].length)
                            {
                                maps[map][index] = new MappedColor(r, g, b,
                                    index);
                            }
                        }
                    }
                    catch(NumberFormatException e)
                    {
                        AExceptionHandler.processException("Colour map error:\n" +
                            mapping, e);
                    }
                } // if
            } // for
        }
        catch(Exception e)
        {
            AExceptionHandler.processException("Error while reading colour map:\n" +
                                               fileName, e);
        }
        for(int i = 0; i < maps.length; i++)
        {
            for(int j = 0; j < maps[i].length; j++)
            {
                if(maps[i][j] == null)
                {
                    AOutput.append("\nColorMap (" + i + "," + j +
                                   ") is not present",
                                   ALogInterface.BAD_COMMAND);
                }
            }
        }

    } // readColorMap() -----------------------------------------------------



    public static int getNumMaps()
    {
        return numMaps;
    }


    public static int getNumColors()
    {
        return numColors;
    }


    /**
     * Returns color map in the XML format.
     * [Former method writeColorMap() was writing the XML data directly into
     * the file.]
     * @return String
     */
    public static String getColorMapXML()
    {
        StringBuffer buffer = new StringBuffer();
        // color map DTD
        buffer.append(
            "<?xml version=\"1.0\"?>\n" +
            "<!DOCTYPE colormap  [\n" +
            " <!ELEMENT colormap (Mapping*)>" +
            " <!ATTLIST colormap\n" +
            "   numMaps   CDATA #REQUIRED\n" +
            "   numColors CDATA #REQUIRED>\n" +
            " <!ELEMENT Mapping EMPTY>\n" +
            " <!ATTLIST Mapping\n" +
            "   map   CDATA #REQUIRED\n" +
            "   index CDATA #REQUIRED\n" +
            "   r     CDATA #REQUIRED\n" +
            "   g     CDATA #REQUIRED\n" +
            "   b     CDATA #REQUIRED>\n" +
            "]>\n\n\n");
        buffer.append("<colormap numMaps=\"" + maps.length +
                      "\" numColors=\"" + maps[0].length + "\" >\n");
        for(int i = 0; i < maps.length; i++)
        {
            for(int j = 0; j < maps[i].length; j++)
            {
                int r = maps[i][j].getRed();
                int g = maps[i][j].getGreen();
                int b = maps[i][j].getBlue();
                buffer.append("  <Mapping  map=\"" + i + "\" index=\"" + j +
                              "\"  r=\"" + r + "\" g=\"" + g + "\" b=\"" +
                              b + "\"/>\n");
            }
        }
        buffer.append("</colormap>\n");
        return buffer.toString();

    } // getColorMapXML() ---------------------------------------------------



    public static String getPSPrologColorMap()
    {
        StringBuffer s = new StringBuffer();
        String newLine = System.getProperty("line.separator");
        s.append(newLine);
        for(int i = 0; i < maps.length; ++i)
        {
            s.append("/" + mapNamesPS[i] + "ColorMap" + newLine);
            s.append("<<" + newLine);
            for(int j = 0; j < maps[i].length; ++j)
                s.append(" /C" + j + " [" +
                         String.format("%.3f",maps[i][j].getRed() / 255.) + " "
                         + String.format("%.3f",maps[i][j].getGreen() / 255.) + " " +
                         String.format("%.3f",maps[i][j].getBlue() / 255.)
                         + "]" + newLine);
            s.append(">> def" + newLine);
            s.append(newLine);
        }
        return s.toString();

    } // getPSPrologColorMap() ----------------------------------------------



    public static Color[] getColors(int map)
    {
        Color[] col = new Color[maps[map].length];
        for(int i = 0; i < col.length; ++i)
            col[i] = (Color) maps[map][i];
        return col;
    }

    //From hot (red), index 255, to cold (blue), index 0
    public static Color[] getShades(int num)
    {
        assert (num>=2);
        Color[] col = new Color[num];
        col[0]=new Color(170,170,170);//grey for "other"
        for(int i = 1; i < num-1; ++i)
            col[i] = new Color(((255*i)/num),80,255-((255*i)/num));
        return col;
    }

    public static Color[] getColors()
    {
        return getColors(currentMap);
    }


    public static void setColorMap(int map)
    {
        if(map >= 0 && map < maps.length)
        {
            currentMap = map;
        	APar parameterStore = APar.instance();
            parameterStore.get("Prefs","ColorMap").setI(map);
        }
    }


    public static void setColorMap(String mapName)
    {
        for(int i = 0; i < mapNames.length; i++)
            if(mapNames[i].equals(mapName))
                setColorMap(i);
    }


    public static int getColorMap()
    {
        return currentMap;
    }

    public static boolean drawFrames()
    {
        if(currentMap==COLOR_MAP_GRAY  || currentMap==COLOR_MAP_BW)
            return true;
        else
            return false;
    }
    
    public static String getPSColorMapName()
    {
        return mapNamesPS[getColorMap()];

    } // getPSColorMapName() ------------------------------------------------


    public MappedColor getMappedColor(int colorIndex)
    {
        if(colorIndex < 0 && colorIndex >= maps[currentMap].length)
        {
            return null;
        }
        else
        {
            return maps[currentMap][colorIndex];
        }
    }


    public Color getColor(int colorIndex)
    {
        if(colorIndex < 0 && colorIndex >= maps[currentMap].length)
        {
            return null;
        }
        else
        {
            return(Color) maps[currentMap][colorIndex];
        }
    }


    public static String getTag(int colorIndex)
    {
        if(colorIndex < 0 && colorIndex >= maps[0].length)
        {
            return null;
        }
        else
        {
            return "C" + colorIndex;
        }
    }


    public static String[] getMapNames()
    {
        return(String[]) mapNames.clone();
    }
    
    public static MappedColor[][] getMaps() {
    	return maps;
    }

} // class AColorMap ========================================================
