package atlantis.gui;

import java.awt.Frame;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.RasterFormatException;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.filechooser.FileFilter;

import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.canvas.ACanvas;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AUtilities;
import atlantis.config.ADefaultValues;
import atlantis.globals.AGlobals;
import atlantis.graphics.encoders.AImageEncoder;
import atlantis.utils.ALogger;


/**
 * Dialog window for saving a picture of the current canvas. This dialog allows
 * the user to pick the resolution and file type. 
 *
 * @author Eric Jansen
 * @author Sebastian Boeser
 */
public class ASaveCanvasDialog extends JFileChooser
  implements PropertyChangeListener {
  
    private final ALogger logger = ALogger.getLogger(ASaveCanvasDialog.class);
    private static AEventManager eventManager = AEventManager.instance();
	private static final AGlobals globals = AGlobals.instance();

    // GUI component that contains the resolution selection panel.
    private AResolutionChooser resolutionChooser;

    // Possible image widths shown to user. Heights are fixed by layout restrictions.
    private final int[] IMAGE_WIDTHS = {800, 1024, 2400, 5000};//{512, 800, 1024, 2400};

    // Default selected image width. Also the (fixed) width of the EPS images.
    private final int DEFAULT_IMAGE_WIDTH = 1;

    // String holding the default file name, need to be reset after change of file type.
    private String defaultFileName = null;

    // Private image encoder instance
    private final AImageEncoder imageEncoder = new AImageEncoder();


    /**
     * Constructor creating the dialog
     */
    public ASaveCanvasDialog()
    {
      //Initialize the file chooser dialog
      super(ADefaultValues.get("LastCanvasPlotSaveDir"));
      setDialogTitle("Save Canvas");

      //Add file filters
      addChoosableFileFilter(new AFileFilter("eps", "Encapsulated Postscript (*.eps)"));
      addChoosableFileFilter(new AFileFilter("png", "Portable Network Graphics (*.png)"));
      addChoosableFileFilter(new AFileFilter("svg", "Scalable Vector Graphics (*.svg)"));

      //Set default values
      setAcceptAllFileFilterUsed(false);
      // In OpenJDK JRE filter is set to null by setAcceptAllFileFilterUsed, so
      // have to set it to something:
      setFileFilter(getChoosableFileFilters()[1]);
      
      //Add the resoluton chooser
      resolutionChooser = new AResolutionChooser();
      setAccessory(resolutionChooser);
      addPropertyChangeListener(this);

      //Set a default file name and extension
      defaultFileName = makeEventFileName() + "-" + AUtilities.getDateTimeString();
      String ext = ((AFileFilter) getFileFilter()).getExtension();
      setSelectedFile(new File(defaultFileName + "." + ext));
    }

    /**
     * Pops up the "Save Canvas" dialog.
     */
    public void showDialog(){
      //Show dialog with GUI as parent
      showSaveDialog(globals.getGuiFrame());
    }


    /**
     * @return the defualt file name
     */
    public String getDefaultFileName()
    {
      return defaultFileName;
    }

    /**
     * Get the currently selected image width.
     * @return selected image width
     */
    public int getImageWidth()
    {
        return resolutionChooser.getImageWidth();
    }



    /**
     * Gets the image height on this button.
     * @return image height
     */
    public int getImageHeight()
    {
        return resolutionChooser.getImageHeight();
    }



    /**
     * Called when the user has selected a file and pushes the save button.
     * This function will only
     * approve the selection when the actual saving was successful.
     */
  @Override
    public void approveSelection()
    {
	  	Frame gui = globals.getGuiFrame();
        File file = new File(getSelectedFile().getAbsolutePath());

        // If the file exists and the user does not want to overwrite it, cancel the save operation.
        // The dialog is still on screen and the user can pick another file.
        if (file.exists() && JOptionPane.showConfirmDialog(gui, "File exists, overwrite?",
                "File exists", JOptionPane.YES_NO_OPTION) != JOptionPane.YES_OPTION)
            return;

        // All exceptions are handled equally: the user is sent back to the save canvas dialog.
        try {
            String type = ((AFileFilter)getFileFilter()).getExtension();
            // Since EPS files can be pretty big, they're saved directly
            if (type.equals("eps")){
              // Since vector images are scalable, the image size only determines the relative size of
              // the data objects. To make the generated EPS independent of the canvas size, we always
              // rescale the image to the default size.
              int width = IMAGE_WIDTHS[DEFAULT_IMAGE_WIDTH];
              int height = ACanvas.getCanvas().getRespectiveHeight(width);
              imageEncoder.saveEPS(width,height,file);
            } else if (type.equals("png")) {
              // Generate the canvas image in memory
              byte[] data;
              int width = resolutionChooser.getImageWidth();
              int height = resolutionChooser.getImageHeight();
              data = imageEncoder.getPNGData(width,height);
              // And finally write the image to the selected file.
              OutputStream stream = new BufferedOutputStream(new FileOutputStream(file.getAbsolutePath()));
              stream.write(data);
              stream.close();
            } else if (type.equals("svg")){
                // Since vector images are scalable, the image size only determines the relative size of
                // the data objects. To make the generated EPS independent of the canvas size, we always
                // rescale the image to the default size.
                int width = IMAGE_WIDTHS[DEFAULT_IMAGE_WIDTH];
                int height = ACanvas.getCanvas().getRespectiveHeight(width);
                imageEncoder.saveSVG(width,height,file);
              } else{
              //Throw an exception if none of the above
              throw new AAtlantisException("Unknown image type: '" + type + "'");
            }
        }
        catch(RasterFormatException e)
        {
            JOptionPane.showMessageDialog(gui,
                    "The chosen resolution for canvas is too high, try a lower option!",
                    "Error saving canvas", JOptionPane.INFORMATION_MESSAGE);
            AGUI.getGUI().restoreDefaults();
            this.cancelSelection();
        } catch(Exception e)
        {
            // Show a dialog with the error message.
            JOptionPane.showMessageDialog(AGUI.getGUI(), e.getMessage(),
                    "Error saving canvas", JOptionPane.ERROR_MESSAGE);

            // The selection is not approved, the user has to select another file or type.
            return;
        }

        // This approves the selection and ends the saving processs.
        super.approveSelection();
        ADefaultValues.set("LastCanvasPlotSaveDir", file.getParent() + 
        		System.getProperty("file.separator"));

    }

    

    /**
     * Property change listener for the JFileChooser
     * @param e property change event sent by JFileChooser
     */
    public void propertyChange(PropertyChangeEvent e)
    {
        if (e.getPropertyName().equals(JFileChooser.FILE_FILTER_CHANGED_PROPERTY))
        {
            String oldExt = ((AFileFilter)e.getOldValue()).getExtension();
            String newExt = ((AFileFilter)e.getNewValue()).getExtension();

            if ((oldExt.equals("eps") || oldExt.equals("svg")) && !(newExt.equals("eps") || newExt.equals("svg")))
            {
                for (int i=0; i<resolutionChooser.getComponentCount(); i++)
                {
                    resolutionChooser.getComponent(i).setEnabled(true);
                }
            }
            else if (!(oldExt.equals("eps") || oldExt.equals("svg")) && ((newExt.equals("eps")) || newExt.equals("svg")))
            {
                for (int i=0; i<resolutionChooser.getComponentCount(); i++)
                    resolutionChooser.getComponent(i).setEnabled(false);
            }

            // The behaviour of the JFileChooser is somewhat strange. Regardless of what the user has entered,
            // the input text field will be emptied when the user selects another file type. It is impossible
            // to retrieve the text that was in this field, so we just reset the default file name.
            if (getSelectedFile() == null)
            {
                // Reset our default with the new extension
                setSelectedFile(new File(defaultFileName + "." + newExt));
                updateUI();
                rescanCurrentDirectory();
            }
        }
    }



    /**
     * Generates a file name based on the current event name and the visible projections.
     * @return auto generated image file name
     */
    private static String makeEventFileName()
    {
        String name = "Untitled";
        AEvent ev = eventManager.getCurrentEvent();
        if(ev != null)
        {
            name = ev.getSourceName();
            if(name.toLowerCase().endsWith(".xml"))
            {
                // current event comes from XML file, run and event number
                // are probably already part of the current filename
                name = name.substring(0, name.length() - 4);
            }
            else
            {
                // current event was probably retrieved online, add run and
                // event numbers into the file name of the canvas plot
                if(! ("n/a".equals(ev.getRunNumber()) &&
                      "n/a".equals(ev.getEventNumber())))
                {
                    name += "_" + ev.getRunNumber() + "_" + ev.getEventNumber();
                }
            }
        }

        ACanvas canvas = ACanvas.getCanvas();
        String[] windows = canvas.getCurrentLayout().getWindowNames();
        StringBuilder listWindows = new StringBuilder();
        for(int i = 0; i < windows.length; i++)
        {
            if(canvas.isReallyOnScreen(canvas.getWindow(windows[i])))
            {
                listWindows.append("-" + canvas.getWindow(windows[i]).getProjection().getName());
            }
        }
        name += listWindows.toString();

        return name;
    }



    /**
     * GUI component for the resolution selection panel.
     */
    class AResolutionChooser extends JPanel implements ActionListener
    {
        /** Currently selected button. */
        private AResolutionButton selectedButton;

        /**
         * Constructor.
         */
        public AResolutionChooser()
        {
            super(new GridLayout(IMAGE_WIDTHS.length+1, 1));
            setBorder(BorderFactory.createTitledBorder(" Resolution: "));

            ButtonGroup group = new ButtonGroup();
            for (int i=0; i<IMAGE_WIDTHS.length; i++)
            {
                int width = IMAGE_WIDTHS[i];
                int height = ACanvas.getCanvas().getRespectiveHeight(IMAGE_WIDTHS[i]);

                AResolutionButton button = new AResolutionButton(width, height);
                button.addActionListener(this);
                if(i == DEFAULT_IMAGE_WIDTH)
                {
                    selectedButton = button;
                    button.setSelected(true);
                }
                group.add(button);
                add(button);
            }
        }

        /**
         * Get the currently selected image width.
         * @return selected image width
         */
        public int getImageWidth()
        {
            return selectedButton.getImageWidth();
        }

        /**
         * Gets the currently selected image height.
         * @return selected image height
         */
        public int getImageHeight()
        {
            return selectedButton.getImageHeight();
        }

        public void updateResolutions()
        {
            for (int i=0; i<getComponentCount(); i++)
            {
                if (getComponent(i) instanceof AResolutionButton)
                {
                    AResolutionButton button = (AResolutionButton)getComponent(i);
                    int height = ACanvas.getCanvas().getRespectiveHeight(button.getImageWidth());
                    button.setImageHeight(height);
                }
            }
        }

  

        /**
         * Action listener for the resolution panel.
         * @param e performed action
         */
        public void actionPerformed(ActionEvent e)
        {
            selectedButton = (AResolutionButton)e.getSource();
        }

        /**
         * Radio button for the resolution chooser.
         */
        public class AResolutionButton extends JRadioButton
        {
            /** Image width for this button. */
            int imageWidth;
            /** Image height for this button. */
            int imageHeight;

            /**
             * Constructor.
             * @param targetWidth image width this button corresponds to
             * @param targetHeight image height this button corresponds to
             */
            public AResolutionButton(int imageWidth, int imageHeight)
            {
                super(imageWidth + "x" + imageHeight);
                this.imageWidth = imageWidth;
                this.imageHeight = imageHeight;
            }

            /**
             * Gets the image width on this button.
             * @return image width
             */
            public int getImageWidth()
            {
                return imageWidth;
            }

            /**
             * Gets the image height on this button.
             * @return image height
             */
            public int getImageHeight()
            {
                return imageHeight;
            }

            public void setImageHeight(int imageHeight)
            {
                this.imageHeight = imageHeight;
                setText(imageWidth + "x" + imageHeight);
            }
        }
    }

    /**
     * Simple filter class that filters files by extension.
     */
    private static class AFileFilter extends FileFilter
    {
        /** File extension. */
        private String extension;
        /** Description of the file type this filter shows. */
        private String description;

        /**
         * Constructs a file filter.
         * @param extension file extension that passes the filter
         * @param description description of the file type this filter selects
         */
        public AFileFilter(String extension, String description)
        {
            super();
            this.extension = extension;
            this.description = description;
        }

        /**
         * Decides if the filter allows a file to pass or not
         * @param f file under consideration
         * @return decision of the filter
         */
        public boolean accept(File f)
        {
          //Accept all directories
          if (f.isDirectory()) return true;

          //Accept all files that end with given extension
          if (f.getName().toLowerCase().endsWith(extension)) return true;

          //Nothing else
          return false;
        }

        /**
         * Get the extension this filter filters on.
         * @return file extension
         */
        public String getExtension()
        {
            return extension;
        }

        /**
         * Get description of the filter.
         * @return description
         */
        public String getDescription()
        {
            return description;
        }
    }

}
