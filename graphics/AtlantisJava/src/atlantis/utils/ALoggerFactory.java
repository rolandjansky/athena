package atlantis.utils;

import org.apache.log4j.Logger;
import org.apache.log4j.spi.LoggerFactory;

/**
 * LoggerFactory as used by ALogger - necessary when subclassing Logger
 * @author Zdenek Maxa
 */
public class ALoggerFactory implements LoggerFactory
{

	/**
	 * The constructor should be public as it will be called by
	 * configurators in different packages.
	 */
	public ALoggerFactory() { }
	
	
	public Logger makeNewLoggerInstance(String name) 
	{
		return new ALogger(name);
	}

} // class ALoggerFactory ===================================================
