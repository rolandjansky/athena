package atlantis.gui;

import java.awt.Frame;
import java.io.File;
import javax.swing.*;
import javax.swing.filechooser.*;

import atlantis.globals.AGlobals;


/**
 * A GUI component used choose a specific XML file
 */
public abstract class AXMLGeometryChooser extends JFileChooser {
	
  private static final AGlobals globals = AGlobals.instance();

  public AXMLGeometryChooser(String fileName) {
    super();
    String FILE_SEPAR = System.getProperty("file.separator");
    Frame gui = globals.getGuiFrame();
    setFileFilter(new XMLFilter());
    setCurrentDirectory(new File(System.getProperty("user.dir")+FILE_SEPAR+
                                 "geometry"));
    if(fileName!=null)
      setSelectedFile(new File(System.getProperty("user.dir")+FILE_SEPAR
          +fileName));
    do {
      int state=showOpenDialog(gui);
      if(state==APPROVE_OPTION) {
        File file=getSelectedFile();
        if(file!=null)
          if(processSelectedFile(file))
            break;
          else {
            JOptionPane.showMessageDialog(gui,
                "Unable to open " + file, "IO Error", JOptionPane.ERROR_MESSAGE);
          }
      } else
        break;
    } while(true);
  }


  public AXMLGeometryChooser() {
    this(null);
  }


  public abstract boolean processSelectedFile(File file);


  private static class XMLFilter extends FileFilter {
    // Accept all directories and all XML files.

    public boolean accept(File f) {
      if(f.isDirectory())
          return true;

      String s=f.getName();
      if(s.startsWith("AMuon")) return false;
      if(s.equals("config.xml")) return false;
      if(s.equals("pdg.xml")) return false;
      if(s.toLowerCase().lastIndexOf(".xml")==s.length()-".xml".length()) return true;
      return false;
    }

    public String getDescription() {
      return ".xml";
    }
  }

}
