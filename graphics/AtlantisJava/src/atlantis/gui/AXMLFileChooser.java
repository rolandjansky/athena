package atlantis.gui;

import java.awt.Component;
import java.io.File;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileFilter;



/**
 * A GUI component used choose a specific XML file
 */
public abstract class AXMLFileChooser extends JFileChooser
{
    private Component parent = null;

    public abstract boolean processSelectedFile(File file);


    public AXMLFileChooser(String fileName, Component parent)
    {
        super();
        this.parent = parent;

        setFileFilter(new XMLFilter());
        if (fileName == null)
        {
            setCurrentDirectory(new File(System.getProperty("user.dir")));
        }
        else
        {
            File f = new File(fileName);
            if (f.isFile())
            {
                setCurrentDirectory(new File(f.getAbsolutePath()));
                setSelectedFile(f);
            }
            else
            {
                setCurrentDirectory(f);
            }
        }

    } // AXMLFileChooser() --------------------------------------------------



    public void showMyDialog()
    {
        do
        {
            int state = showOpenDialog(this.parent);
            if(state == APPROVE_OPTION)
            {
                File file = getSelectedFile();
                if(file != null)
                {
                    if(processSelectedFile(file))
                    {
                        break;
                    }
                    else
                    {
                        JOptionPane.showMessageDialog(parent,
                            "Unable to open\n" + file, "IO Error",
                            JOptionPane.ERROR_MESSAGE);
                    }
                }
            }
            else
            {
                break;
            }
        }
        while(true);

    } // showMyDialog() -----------------------------------------------------



    private static class XMLFilter extends FileFilter
    {
        // Accept all directories and all valid files.
        public boolean accept(File f)
        {
            if(f.isDirectory())
            {
                return true;
            }

            String s = f.getName();

            if(s.toLowerCase().endsWith(".xml") || s.toLowerCase().endsWith(".zip")
                    || s.toLowerCase().endsWith(".gzip") || s.toLowerCase().endsWith(".gz"))
                return true;

            return false;
        }


        public String getDescription()
        {
            return ".xml, .zip, .gzip, .gz";
        }
    }

}
