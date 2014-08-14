package atlantis.gui;

import java.awt.Component;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileFilter;

import atlantis.globals.AGlobals;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AUtilities;

/**
 * Utility methods related to the GUI. Moved from utils.Utilities.
 * 
 * @author waugh
 */
public class AGUIUtilities {

	private static final AGlobals globals = AGlobals.instance();

	/**
	 * Save file dialog. Show the dialog with prepared name of the file in
	 * the directory. Returns the selected directory.
	 *
	 * @param component JDialog
	 * @param directory String
	 * @param fileName String
	 * @param data byte[]
	 * @param title String
	 * @param ext extension for file filter
	 * return selectedDirectory String
	 */
	public static String chooseFileAndWrite(Component component, String directory,
			String fileName, byte[] data, String title, String ext)
	throws AAtlantisException
	{
		String selectedDirectory = directory;

		if(directory == null)
		{
			directory = AGlobals.instance().getHomeDirectory();
		}

		JFileChooser chooser = new JFileChooser(directory);
		chooser.setDialogTitle(title);
		chooser.setSelectedFile(new File(fileName));
		SaveDialogFileFilter filter = new SaveDialogFileFilter(ext);
		chooser.setFileFilter(filter);

		int returnVal = chooser.showSaveDialog(component);

		if(returnVal == JFileChooser.APPROVE_OPTION)
		{
			String absPath = chooser.getSelectedFile().getAbsolutePath();
			File f = new File(absPath);
			if(f.exists())
			{
				try
				{
					renameFile(absPath, title);
				}
				catch(Exception ex)
				{
					throw new AAtlantisException(ex.getMessage());
				}
			}
			// the selected file either existed and is now successfully
			// renamed, or didn't exist so can proceed to store data
			try
			{
				OutputStream  s =
					new BufferedOutputStream(new FileOutputStream(absPath));
				s.write(data);
				s.close();
				selectedDirectory = chooser.getSelectedFile().getParent();
			}
			catch(FileNotFoundException ex)
			{
				String msg = "Couldn't create the file.\nInsufficient permissions.";
				throw new AAtlantisException(msg);
			}
			catch(IOException ex)
			{
				String msg = "IO Error when writing the\ndata into the file.";
				throw new AAtlantisException(msg);
			}
		}
		return selectedDirectory;

	} // chooseFileAndWrite() -----------------------------------------------

	
	/**
	 * renames file (backup when saving configuration, canvas plot, etc)
	 * to 'name' + backup suffix
	 */
	private static void renameFile(String name, String title) throws Exception
	{
		String backupSuffix = "-backup_on_" + AUtilities.getDateTimeString();
		String dstName = name + backupSuffix;
		String errorMsg = "Renaming\n" + name + "\nto\n" + dstName + "\nfailed.";
		String msg = name + "\nexists and was successfully renamed to\n" + dstName;

		try
		{
			AUtilities.logger.debug("Renaming file " + name + " to " + dstName + " ...");
			File f = new File(name);
			if(! f.renameTo(new File(dstName)))
			{
				AUtilities.logger.error(errorMsg);
				throw new Exception(errorMsg);
			}
			AUtilities.logger.debug(msg);
			JOptionPane.showMessageDialog(globals.getGuiFrame(), msg, title,
					JOptionPane.INFORMATION_MESSAGE);
		}
		catch(Exception ex)
		{
			AUtilities.logger.debug(ex.getMessage(), ex);
			throw new Exception(errorMsg);
		}

	} // renameFile() -------------------------------------------------------

	
	private static class SaveDialogFileFilter extends FileFilter
	{
		private String extension = null;

		public SaveDialogFileFilter(String acceptedExtension)
		{
			extension = acceptedExtension;
		}

		// accept all directories and all files of "extension"
		public boolean accept(File f)
		{
			if (f.isDirectory())
			{
				return true;
			}

			String s = f.getName();

			if(s.toLowerCase().endsWith(extension))
			{
				return true;
			}

			return false;
		}

		public String getDescription()
		{
			return extension;
		}

	} // SaveDialogFileFilter ===================================================

}
