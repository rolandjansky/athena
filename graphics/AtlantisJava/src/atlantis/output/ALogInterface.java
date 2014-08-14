package atlantis.output;

/**
 * Non-GUI-dependent interface for ALogPane.
 * 
 * @author waugh
 *
 */
public interface ALogInterface {

	public static final String TITLE = "TITLE";
	public static final String NORMAL = "NORMAL";
	public static final String NORMAL_BOLD = "NORMAL_BOLD";
	public static final String COMMAND = "COMMAND";
	public static final String BAD_COMMAND = "BAD_COMMAND";
	public static final String PICK = "PICK";
	public static final String WARNING = "WARNING";

	void append(String s);

	void append(String s, String style);

	void clear();

}