package atlantis.event;

import atlantis.utils.*;
import java.io.File;
import java.io.IOException;

import atlantis.canvas.ACanvas;
import atlantis.graphics.encoders.AImageEncoder;
import java.awt.Dimension;


/** 
 * Implements ANewEventListener and creates PNG files
 * upon each newEvent call. A scale factor allows to e.g. produce thumbnails by
 * scaling down image. The image producer works in two modes:
 * a) If no file name is given, a file name will be generated from run and event
 *    number. In case the PNG file exists, no file is created but a warning is written out.
 * b) If a file name is given, a temporary file is generated first, and then
 *    renamed to the given file name. If renaming fails, a warning is written out
 *    and the temporary file is deleted.
 * 
 * @author Sebastian Boeser
 */

public class APngImageProducer extends AImageProducer
{
	// the logger
	private static final ALogger logger = ALogger.getLogger(APngImageProducer.class);

	// Our image encode
	private final AImageEncoder imageEncoder = new AImageEncoder();

	/**
	 * Fully qualified constructor, 
	 * intializing all the attributes and check the parameters validity
	 * @param dir the directory in which to save all the files
	 * @param size the dimensions of the images
	 * @param scale the factor by which to scale the images
	 * @param fileName if not null, that file will be overwritten for each event
	 * @throws InstantiationException if we fail to create the object
	 */
	public APngImageProducer(String dir, Dimension size, double scale, String fileName) throws InstantiationException
	{
		super(dir, size, scale, fileName);
	}

	/**
	 * Called whenever a new event appears. Store a PNG in the directory,
	 * either with a generated file name  (no overwrite) or with a fixed file name (overwrite)
	 * @param event the event from which to generate the PNG
	 */
	public void newEvent(AEvent event)
	{

		//If auto had been set for size (height is negative), get height from Canvas
		if (imageSize.height < 0) imageSize.setSize(imageSize.width, ACanvas.getCanvas().getRespectiveHeight(imageSize.width));

		//Create the file to write the data into
		File pngFile;
		//If we don't have a fixed file name,
		//generate a unique one based on run- and event number
		if (fixedFileName==null){

			//Now construct all the full path for the file we want to save
			String pngName = String.format("%s%s%s.png",directory,
					System.getProperty("file.separator"),
					getEventFileName(event));

			// Create file handles to the file and check if it exists
			pngFile = new File(pngName);
			//Do not overwrite any existing files
			if (pngFile.exists()) {
				logger.warn("File "+ pngName +" already exists - will not overwrite");
				return;
			}
			//otherwise generate a temporary file first,
			//then move it in place later
		} else {
			//make a temporary file in the final directory
			try {
				pngFile = File.createTempFile(filePrefix+"_", ".png",directory);
			} catch (IOException ex) {
				logger.warn("Failed to create temporary file in "+directory.getAbsolutePath());
				return;
			}
		}

		//Now create the image. If the scaleFactor equals 1, no scaling will happen.
		byte[] pngData = imageEncoder.getScaledPNGData(imageSize.width,imageSize.height,scaleFactor);

		//Now try saving the acquired data,
		//and if it shall go to a fixed file, move it there
		try {
			//Save the PNG
			saveDataIntoFile(pngData, pngFile);
			//move to fixed file name if requested
			if (fixedFileName != null){

				//Get a handle to the final destination
				File fixedFile = new File(directory,fixedFileName);

				//delete target if exits
				if (fixedFile.exists() && (!fixedFile.delete()))
					throw new IOException("Failed to delete existing file "+fixedFile.getAbsolutePath());

				//And move the new file in place
				if (!pngFile.renameTo(fixedFile))
					throw new IOException("Failed to rename temporary file to "+fixedFile.getAbsolutePath());
			}
		} catch (IOException ioe) {
			//If we fail, throw a warning
			logger.warn("Could not save PNG files for history\n"+ioe.toString());
		} finally {
			//In any case, delete the pngFile, if it was just temporary
			if (fixedFileName != null) pngFile.delete();
		}

	} 	
}
