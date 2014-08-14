package atlantis.config;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.FileReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Enumeration;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import atlantis.globals.AGlobals;
import atlantis.utils.ALogger;

/**
 *    The class maintains a few values which are to be remembered
 *    from session to session. The class tries to read in values from runtime
 *    configuration file $HOME/.Atlantis-runtime_values.txt. The values are
 *    stored in a Hashtable and are used as default initialize values. This
 *    Hashtable is updated whenever user changes these values. Values are
 *    into file at the end of Atlantis session.
 *
 * TODO:
 *      last used geometry (will be simpler when TB mode (and Atlantis.mode in
 *           general is discontinued), geometry names will be considered
 *           after '_' up to the file extension (.xml)
 *      last event file
 *
 * @author Zdenek Maxa
 */
public class ADefaultValues
{
    // file where the cross-session runtime values are stored
    public static final String CONFIGFILE = System.getProperty("user.home") +
    		System.getProperty("file.separator") +
    		".Atlantis-runtime_values.txt";
    // main attribute which holds all data
    private static Hashtable v = new Hashtable();

    private static ALogger logger = ALogger.getLogger(ADefaultValues.class);

    private ADefaultValues() {}



    /**
     * Variables controled by this class and their default values if the
     * runtime config does not exist or if some values are missing in the file.
     */
    private static void setDefaultValues()
    {
    	String FILE_SEPAR = System.getProperty("file.separator");
    	String homeDirectory = AGlobals.instance().getHomeDirectory();
        v.put("LastEventFilesSourceDir", homeDirectory + "events" + FILE_SEPAR);
        v.put("LastEventFilesSaveDir", homeDirectory);
        v.put("LastCanvasPlotSaveDir", homeDirectory);
        v.put("JiveXMLServerName", "");
        v.put("JiveXMLServerPort", "48965");
        v.put("GeometryName", "");

    } // setDefaultValues() -------------------------------------------------



    private static Hashtable readRuntimeConfigFile()
    {
        Hashtable helpTable = null;
        String line = null;
        String[] l = null;
        String patternLine = ".*=.*";
        Pattern pattern = Pattern.compile(patternLine);

        try
        {
            BufferedReader in = new BufferedReader(new FileReader(CONFIGFILE));
            helpTable = new Hashtable();
            logger.info("Atlantis runtime configuration file: " + CONFIGFILE);

            while((line = in.readLine()) != null)
            {
                Matcher matcher = pattern.matcher(line);
                if(matcher.matches())
                {
                    l = line.split("=");
                    if(l != null && l.length == 2)
                    {
                        logger.debug("Configuration value: " +
                                          l[0] + " : " + l[1] + " read in");

                        // remove quotes
                        l[1] = l[1].replaceAll("\"", "");
                        helpTable.put(l[0].trim(), l[1].trim());
                    }
                    else
                    {
                        logger.warn("Line: " + line + " from runtime " +
                                      "configuration file: " + CONFIGFILE +
                                      " does not match, line is ignored");
                    }
                }
            } // while

            // file is successfully read in a helper Hash table
            in.close();

            return helpTable;

        } // try
        catch(FileNotFoundException ex)
        {
            logger.warn("Runtime configuration file: " + CONFIGFILE +
                          " not found, using default runtime values");
        }
        catch(IOException ex)
        {
            logger.error("I/O error while reading " + CONFIGFILE +
                          " file, using default runtime values");
        }

        return null;

    } // readRuntimeConfigFile() --------------------------------------------



    public static void readRuntimeValues()
    {
        setDefaultValues();
        Hashtable help = readRuntimeConfigFile();
        if(help != null)
        {
            for(Enumeration e = v.keys() ; e.hasMoreElements() ;)
            {
                String s = (String) e.nextElement();
                if(help.containsKey(s))
                {
                    // keys match, update the value
                    v.remove(s);
                    v.put(s, help.get(s));
                }
            }
        }

    } // readRuntimeValues() ------------------------------------------------



    public static void saveRuntimeValues()
    {
        try
        {
            // if exists, shall be overwritten
            BufferedWriter out = new BufferedWriter(new FileWriter(CONFIGFILE));

            for(Enumeration e = v.keys() ; e.hasMoreElements() ;)
            {
                String s = (String) e.nextElement();
                out.write(s + " = \"" + v.get(s) + "\"\n");
            }
            out.close();
        }
        catch(IOException ex)
        {
            logger.error("Can't open " + CONFIGFILE + " for writing, runtime " +
                          "configuration values not saved");
        }

    } // saveRuntimeValues() ------------------------------------------------



    public static String get(String key)
    {
        if(v.containsKey(key))
        {
            return(String) v.get(key);
        }
        else
        {
            logger.warn("Runtime configuration value: " + key + " does not exist", new Exception());
            return null;
        }

    } // get() --------------------------------------------------------------



    public static void set(String key, String value)
    {
        if(v.containsKey(key))
        {
            v.remove(key);
            v.put(key, value);
        }
        else
        {
            logger.warn("Runtime configuration value: " + key + " does not exist", new Exception());
        }

    } // set() --------------------------------------------------------------



} // class ADefaultValues ===================================================
