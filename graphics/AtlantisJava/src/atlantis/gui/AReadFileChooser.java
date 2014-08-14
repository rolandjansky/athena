package atlantis.gui;

import java.io.File;

import javax.swing.JFileChooser;
import javax.swing.filechooser.FileFilter;
import javax.swing.JCheckBox;
import javax.swing.JPanel;

import java.awt.Component;
import java.awt.BorderLayout;

import atlantis.config.ADefaultValues;

/**
 * This dialog appears when a user asks to read events from a file
 * @author sboeser
 */
public class AReadFileChooser extends JFileChooser {
  //Check box indicating to set the new default location  
  private JCheckBox defaultLoc = null;

  /**
   * Constructor with a default path
   * @param path the default path
   */
  public AReadFileChooser(String path) {

    //Create a file chooser pointing to path
    //Use users default dir if null
    super(path);

    //Create a new panel with the checkbox
    JPanel p = new JPanel();
    defaultLoc = new JCheckBox("default location");
    p.setLayout(new BorderLayout());
    p.add(defaultLoc, BorderLayout.SOUTH);
    setAccessory(p);


    setFileFilter(new XMLFilter());
  }

  /**
   * Show the dialog waiting for user input
   */
  @Override
  public int showOpenDialog(Component parent) {

    //Get the return state 
    int state = super.showOpenDialog(parent);
    
    //Store the default location if user pressed ok
    if (state == APPROVE_OPTION) {
      //Only if checkbox is ticked
      if (defaultLoc.isSelected()) {
        // save last visited location
        ADefaultValues.set("LastEventFilesSourceDir",
                getSelectedFile().getParent() + System.getProperty("file.separator"));
      }
    }
    
    //return the result state
    return state;
  }
  
 /**
  * A filter for XML files
  */
  private static class XMLFilter extends FileFilter {
    
    /**
     * Acceptance algorithm for xml files and directories
     * @param file the file to test
     * @return true if accepted
     */
    public boolean accept(File file) {
      
      //accept all directories
      if (file.isDirectory()) return true;

      //check the file ending
      String name = file.getName().toLowerCase();

      if (name.endsWith(".xml")) return true;
      if (name.endsWith(".zip")) return true;
      if (name.endsWith(".gzip")) return true;
      if (name.endsWith(".gz"))  return true;
     
      //None of the requested criteria matched
      return false;
    }

    /**
     * Gives a description of the files that are accepted by this filter
     * @return string with list of file endings
     */
    public String getDescription() {
      return ".xml, .zip, .gzip, .gz";
    }
  }  
}

