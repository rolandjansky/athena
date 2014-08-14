package atlantis.event;

import atlantis.utils.*;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import atlantis.Atlantis;
import atlantis.canvas.ACanvas;
import atlantis.graphics.encoders.AImageEncoder;
import java.awt.Dimension;


/** 
 * Implements ANewEventListener and creates image files
 * upon each newEvent call. A scale factor allows to e.g. produce thumbnails by
 * scaling down image. The image producer works in two modes:
 * a) If no file name is given, a file name will be generated from run and event
 *    number. In case the output file exists, no file is created but a warning is written out.
 * b) If a file name is given, a temporary file is generated first, and then
 *    renamed to the given file name. If renaming fails, a warning is written out
 *    and the temporary file is deleted.
 * 
 * @author Sebastian Boeser
 */

public abstract class AImageProducer implements ANewEventListener
{
  /** File name prefix */
  protected static final String filePrefix = "atlantis";
  /**  Minimum number of digits for event- and run-number when writing files */
  protected static final int encodeNumberDigits = 6;
	    
  /** directory to store files in */
  protected final File directory ;
  /** Default image dimensions */
  protected final Dimension imageSize;
  /** The thumbnail scale */
  protected final double scaleFactor;
  /** A fixed filename if given on the command line */
  protected final String fixedFileName;

  /**
   * Inner exception class
   */
  public static class InstantiationException extends Exception {
    //only declare constructor
    InstantiationException(String what){
      super(what);
    }
  };
  
	/**
   * Fully qualified constructor, 
	 * intializing all the attributes and check the parameters validity
   * @param dir the directory in which to save all the files
   * @param size the dimensions of the images
   * @param scale the factor by which to scale the images
   * @param fileName if not null, that file will be overwritten for each event
   * @throws InstantiationException if we fail to create the object
	 */
	protected AImageProducer(String dir, Dimension size, double scale, String fileName) throws InstantiationException
	{
    // First check the directory parameter
		directory = new File(dir);
    
    //That it exists
		if(! directory.exists()) throw new InstantiationException("Directory "+dir+" doesn't exist");

    //That it is a directory
    if(! directory.isDirectory()) throw new InstantiationException(dir + " is not a directory");
		
    //And that we can write in it
    if(! directory.canWrite()) throw new InstantiationException("Cannot write to directory "+dir);

    //Check frequency and history length arguments
    if (scale <= 0) throw new InstantiationException("Negative or zero scale factor");

    // store all parameters
    imageSize = size;
    scaleFactor = scale;
    fixedFileName = fileName;

	}
	

	/**
	 * Return the string build from this events file name
	 * @param evt the event info from which to build the filename
	 * @param minDigit minimum number of digits for run- and event-number
	 * @return the file name
	 */
	protected String getEventFileName(AEventInfo evt){
		String nameFormat = String.format("%%s_%%0%dd_%%0%dd",encodeNumberDigits,encodeNumberDigits);
		return String.format(nameFormat,
				filePrefix,evt.getRunNumber(),evt.getEventNumber());
	}
	
	
  /**
   * Safes a byteArray of data into a file given by a fileName
   * @param data the data
   * @param file the file to write into
   * @throws Exception in case of saving fails
   */
	protected void saveDataIntoFile(byte[] data, File file) throws IOException
	{
	
    //Sanity check
    if ((data == null) ||(file == null)) return;
    
    //Open output stream and write to output stream
    FileOutputStream fos = new FileOutputStream(file);
	  OutputStream os = new BufferedOutputStream(fos);
	  os.write(data);
    os.flush();
	  os.close();
		
	}
  
  /**
   * Called whenever a new event appears. Store an image in the directory,
   * either with a generated file name  (no overwrite) or with a fixed file name (overwrite)
   * @param event the event from which to generate the image
   */
	public abstract void newEvent(AEvent event);
}