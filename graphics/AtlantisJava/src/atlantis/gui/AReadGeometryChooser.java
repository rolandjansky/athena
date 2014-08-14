package atlantis.gui;

import atlantis.geometry.AEndcapTRTDetector;

import java.awt.Frame;
import java.io.File;
import javax.swing.JOptionPane;

import atlantis.canvas.ACanvas;
import atlantis.data.ACalorimeterData;
import atlantis.geometry.AAtlasDetector;
import atlantis.geometry.ABarrelSiliconDetector;
import atlantis.geometry.ABarrelTRTDetector;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.geometry.AEndcapSiliconDetector;
import atlantis.globals.AGlobals;


/**
 * The dialog which appears when a user asks to read in geometry XML file
 */
public class AReadGeometryChooser extends AXMLGeometryChooser {

  private static final AGlobals globals = AGlobals.instance();

  public AReadGeometryChooser() {
    super();
  }

  public void fileChosen(int state) {
	Frame gui = globals.getGuiFrame();
    do {
      if(state==APPROVE_OPTION) {
        File file=getSelectedFile();

        if(file!=null)
          if(file.canRead()) {
            processSelectedFile(file);
            break;
          } else {
            JOptionPane.showMessageDialog(gui,
                "Unable to open " + file, "IO Error", JOptionPane.ERROR_MESSAGE);
          }
      } else
        break;
      state=showOpenDialog(gui);
    } while(true);
  }

  public boolean processSelectedFile(File file)
  {
      if(file.canRead())
      {
          // ACalorimterDetector keeps a list of its instances, clear this list.
          ACalorimeterDetector.clear();
          ABarrelSiliconDetector.clear();
          AEndcapSiliconDetector.clear();
          ABarrelTRTDetector.clear();
          AEndcapTRTDetector.clear();

          String muonname = file.getParent() + System.getProperty("file.separator") +
                            file.getName().substring(0, 1) + "Muon" +
                            file.getName().substring(1);
          File mufile = new File(muonname);
          if(mufile.canRead())
          {
              AAtlasDetector.setDetector(new AAtlasDetector(file.getAbsolutePath(), muonname));
          }
          else
          {
              AAtlasDetector.setDetector(new AAtlasDetector(file.getAbsolutePath()));
          }
          ACalorimeterData.remakeHitToGeometryMapping();
          ACanvas.getCanvas().repaintAllFromScratch();
          return true;
      }
      return false;
  }
}
