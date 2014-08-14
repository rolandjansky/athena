package testutils;

public class TestUtils {

	/**
	 * Set system property if it doesn't already have a value.
	 * Useful in providing sensible defaults for properties set in build file when
	 * using Ant.
	 * @param key
	 * @param value
	 */
	public static void setPropertyIfNotSet(String key, String value) {
		String old = System.getProperty(key);
		if (old==null) System.setProperty(key,value);
	}

}
