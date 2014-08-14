package atlantis.gui;

import java.awt.geom.Point2D;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.util.Vector;

import javax.swing.JOptionPane;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParametersGroup;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;

/**
 *
 * <ul><li>Reads the configuration file (given file name) and makes the corrections
 * according to the actual configuration which might have been changed by the
 * user.</li>
 * <li>Reads parameters differences (values from the parameters store).</li>
 * <li>Reads canvas / window configuration.</li>
 * <li>Creates XML config files from three above parts.</li></ul>
 *
 * <p>Writes out the current status. This is not a robust piece of code and make
 * break if the format of config.xml is changed by hand.
 * This class should throw an exception if there is a problem.
 * </p>
 *
 * @author Gary Taylor, Zdenek Maxa
 */
public class AConfigWriter
{

	private static ALogger logger = ALogger.getLogger(AConfigWriter.class);
	
	protected static APar parameterStore = APar.instance();

    private AConfigWriter()
    {
    }

    /**
     * Main method from the class
     * @param sourceFile String
     * @return String
     */
    public static String getConfiguration(String sourceFile)
    {
    	logger.trace("Trying to read the distribution configuration ...");
        String config = null;
        StringBuffer output = new StringBuffer("");

        config = readConfiguration(sourceFile);

        if(config != null)
        {
        	logger.debug("Loading source configuration successful.");
            output.append(config);
            output.append(getParametersDifferences());
            output.append(getWindowCorners());
            output.append("\n</AtlantisConfiguration>\n");
            return output.toString();
        }
        else
        {
        	logger.error("Loading source configuration failed.");
            return null;
        }
    } // getConfiguration() -------------------------------------------------



    private static String getParametersDifferences()
    {
        StringBuffer output = new StringBuffer("");

        output.append("\n\n");
        output.append("\t<ParameterDifferences>\n");

        AParametersGroup[][] group = parameterStore.getUIGroups();
        for(int i = 0; i < group.length; i++)
        {
          for(int j = 0; j < group[i].length; j++)
          {
              Vector v = group[i][j].getParameters(999);
              for(int k = 0; k < v.size(); ++k)
              {
                  output.append(((AParameter)v.elementAt(k)).getDifferences(group[i][j].getGroupName()));
              }
          }
        }
        output.append("\t</ParameterDifferences>\n\n");

        return output.toString();

    } // getParametersDifferences() -----------------------------------------



    private static String getWindowCorners()
    {
        StringBuffer output = new StringBuffer("");

        output.append("\n\n");
        output.append("\t<WindowCorners>\n");

        String[] windowNames = ACanvas.getCanvas().getKnownWindowNames();

        for(int i = 0; i < windowNames.length; i++)
        {
            Point2D.Double[] corners =
                ACanvas.getCanvas().getWindow(windowNames[i]).getUserCorners();
            if(corners != null)
            {
                output.append("\t\t<CORNERS windowName=\"" +
                              windowNames[i] + "\"");
                for(int j = 0; j < corners.length; ++j)
                {
                  output.append(" x" + j + "=\"" + corners[j].getX() + "\"");
                  output.append(" y" + j + "=\"" + corners[j].getY() + "\"");
                }
                output.append("/>\n");
            }
        }

        output.append("\t</WindowCorners>\n\n\n");

        return output.toString();
    } // getWindowCorners() -------------------------------------------------


    // Read one logic line from xml file
    // contain <....> or <..../>
    private static String readLogicLine(BufferedReader reader) throws IOException
    {
        String startLine = reader.readLine();
        if(startLine == null)
            return null;
        
        StringBuffer logicLine = new StringBuffer(startLine);
        
        while(logicLine.indexOf("<") >= 0 && logicLine.indexOf(">") < 0)
        {
            logicLine.append("\n");
            logicLine.append(reader.readLine()); 
        }
        return logicLine.toString();
    }
    
    
    private static String readConfiguration(String fileName)
    {
        String groupName = "";
        String parameterName = "";
        AParameter aParam = null;
        BufferedReader curr = null;
        String str;
        StringBuffer buffer = new StringBuffer(""); // working buffer
        StringBuffer output = new StringBuffer(""); // final result
        boolean inComment = false;

        try
        {
        	logger.debug("Trying to open configuration file: " + fileName);
        	InputStream fis = AUtilities.getFileAsStream(fileName);
        	InputStreamReader isr = new InputStreamReader(fis);
        	curr = new BufferedReader(isr, 10000);
            
        	while((str = readLogicLine(curr)) != null)
            {
                buffer = new StringBuffer(str);
                // in a comment ?
                if(buffer.indexOf("<!--") >= 0)
                {
                    inComment = true;
                }
                if(buffer.indexOf("-->") >= 0)
                {
                    inComment = false;
                }
                if(!inComment)
                {
                    // nearly finished writing
                    if(buffer.indexOf("</Parameters>") >= 0)
                    {
                        output.append(buffer);
                        output.append("\n");
                        break;
                    }
                    
                    // modification in the initialisation
                    if(buffer.indexOf("<Canvas") >= 0) {
                        buffer = replace(buffer, "startupLayout", ACanvas.getCanvas().getCurrentLayout().getName());
                        String ar = Double.toString(ACanvas.getCanvas().getAspectRatio());
                        buffer = replace(buffer, "aspectRatio", ar);
                    }
                    
                    if(buffer.indexOf("<UsedWindow") >= 0)
                    {
                        String name = get(buffer, "name");
                        if(name != null)
                        {
                            AWindow w = ACanvas.getCanvas().getWindow(name);
                            if(w != null)
                            {
                                if(w.getProjection() != null)
                                {
                                    buffer = replace(buffer, "projection", w.getProjection().getName());
                                }
                                buffer = replace(buffer, "group", w.getGroupName());
                            }
                        }
                    }

                    if(buffer.indexOf("<Layout") >= 0)
                    {
                        String name = get(buffer, "name");
                        if(name != null && name.equals(ACanvas.getCanvas().getCurrentLayout().getName()))
                        {
                            buffer = replace(buffer, "startupWindow", ACanvas.getCanvas().getCurrentWindowName());
                            buffer = replace(buffer, "startup", ACanvas.getCanvas().getStartupString());
                        }
                    }

                    // new group encountered
                    if(buffer.indexOf("<Group") >= 0)
                    {
                        groupName = get(buffer, "name");
                    }

                    // new parameter encountered
                    aParam = null;
                    if(buffer.indexOf(" fn=\"") >= 0 &&
                       buffer.indexOf("<StatusRoot") < 0 &&
                       buffer.indexOf("<StatusGroup") < 0)
                    {
                        parameterName = removeSpaces(get(buffer, "fn"));
                        aParam = parameterStore.getUnknown(groupName, parameterName);
                    }
                    if(aParam != null)
                    {
                        // replacing status
                        if(buffer.indexOf(" st=\"") >= 0)
                        {
                            String status = "OFF";
                            if(aParam.getStatus())
                            {
                                status = "ON";
                            }
                            buffer = replace(buffer, "st", status);
                        }

                        // replacing value
                        if(buffer.indexOf(" va=\"") >= 0)
                        {
                            String value = aParam.getValue();
                                                        
                            if(parameterName.equals("Area/Energy"))
                            {
                                value = "0.0";
                            }
                            // for instance Track Collections listbox should
                            // always remain:
                            // sn="Track Collections" va="0" pv="none=0"
                            // current 'value' mustn't be put there
                            else if(parameterName.endsWith("Collections"))
                            {
                                value = "0";
                            }
                            buffer = replace(buffer, "va", value);
                                                     
                        }

                        // replacing operator
                        if(buffer.indexOf(" dop=\"") >= 0)
                        {
                            String dop = aParam.getOperator();
                            if(dop.equals("<"))
                            {
                                dop = "&lt;";
                            }
                            if(dop.equals("<="))
                            {
                                dop = "&le;";
                            }
                            if(dop.equals(">"))
                            {
                                dop = "&gt;";
                            }
                            if(dop.equals(">="))
                            {
                                dop = "&ge;";
                            }
                            buffer = replace(buffer, "dop", dop);
                        }

                        // replacing scope
                        if(buffer.indexOf(" scope=\"") >= 0)
                        {
                            if(aParam.getScope() == AParameter.GLOBAL)
                            {
                                buffer = replace(buffer, "scope", "GLOBAL");
                            }
                            else if(aParam.getScope() == AParameter.LOCAL)
                            {
                                buffer = replace(buffer, "scope", "LOCAL");
                            }
                        }
                        else if(buffer.indexOf(" fn=\"") > 0)
                        {
                            // in a parameter - adding scope if needed
                            if(aParam.getScope() != parameterStore.getGroup(groupName).getScope())
                            {
                                int index = buffer.indexOf("/>");
                                if(index > 0)
                                {
                                    if(aParam.getScope() == AParameter.GLOBAL)
                                    {
                                        buffer.insert(index,
                                            " scope=\"GLOBAL\"");
                                    }
                                    else
                                    {
                                        buffer.insert(index,
                                            " scope=\"LOCAL\"");
                                    }
                                }
                            }
                        }
                    } // aParam != null
                } // !inComment
                output.append(buffer);
                output.append("\n");
            } // while
            curr.close();
        } // try
        catch(IOException ex)
        {
        	String m = "I/O error occured when reading the source " +
        	           "configuration file: " + fileName;
        	logger.error(m);
        	logger.debug(m, ex);
            output = null;
        }
    	catch(AAtlantisException ae)
    	{
    		//throw ae;
    	}
        

        return (output != null) ? output.toString() : null;
        
    } // readConfiguration() ------------------------------------------------



   /**
    * Removing spaces from the parameter name
    * @param name String
    * @return String
    */
    private static String removeSpaces(String name)
    {
        StringBuffer strBuffer = new StringBuffer(name);
        int i = 0;
        while(i < strBuffer.length())
        {
            if(Character.isWhitespace(strBuffer.charAt(i)))
            {
                strBuffer.deleteCharAt(i);
            }
            else
            {
                i++;
            }
        }
        return new String(strBuffer);
    } // removeSpaces() -----------------------------------------------------


    private static StringBuffer replace(StringBuffer s, String variable, String text)
    {
        int name = s.indexOf(variable + "=\"");
        if(name >= 0)
        {
            int start = s.indexOf("\"", name);
            int stop = s.indexOf("\"", start + 1);
            s.delete(start + 1, stop);
            s.insert(start + 1, text);
        }
        
        return s;
    } // replace() ----------------------------------------------------------


    private static String get(StringBuffer s, String variable)
    {
        // insert a space before the variable name to prevent from the variable
        // name is a substring of another variable name
        int name = s.indexOf(" " + variable + "=\"");
        if(name >= 0)
        {
            int start = s.indexOf("\"", name);
            int stop = s.indexOf("\"", start + 1);
            return s.substring(start + 1, stop);
        }
        return null;
    } // get() --------------------------------------------------------------

} // class AConfigWriter ====================================================
