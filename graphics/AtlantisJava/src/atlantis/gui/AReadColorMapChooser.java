package atlantis.gui;

import java.io.File;

import atlantis.canvas.ACanvas;
import atlantis.globals.AGlobals;
import atlantis.graphics.colormap.AColorMap;

/**
 * The dialog which appears when a user asks to read a new colormap
 */
public class AReadColorMapChooser extends AXMLFileChooser
{
	private static final String FILE_SEPAR = System.getProperty("file.separator");
	private static final AGlobals globals = AGlobals.instance();
	
    public AReadColorMapChooser()
    {
        super(System.getProperty("user.dir") + FILE_SEPAR
                + "configuration" + FILE_SEPAR + "colormap.xml", globals.getGuiFrame());
    }
    
    public boolean processSelectedFile(File file)
    {
        if (file.canRead())
        {
            AColorMap.readColorMap(file.getPath());
            AColorMapEditor.getInstance().updateColors();
            AGUI.getGUI().repaintTable();
            ACanvas.getCanvas().repaintAllFromScratch();
            return true;
        }
        return false;
    }
}
