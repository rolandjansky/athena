package atlantis.gui;

import java.awt.Component;
import java.io.File;

import atlantis.config.ADefaultValues;

/**
* A dialog which appears when a user asks to read a new event.
*
* @author Qiang Lu
*/
public class ADemoFileChooser extends AXMLFileChooser
{
    private File fileSelected;

    public ADemoFileChooser(String path, Component parent)
    {
        super(path, parent);
    }

    public boolean processSelectedFile(File file)
    {
        if (file.canRead())
        {
            // save last visited location
            ADefaultValues.set("LastEventFilesSourceDir", file.getParent() +
                               System.getProperty("file.separator"));

            fileSelected = file;
            return true;
        }
        return false;
    }

    public File getFileSelected()
    {
        return fileSelected;
    }

}
