package atlantis.parameters;

/**
 * Encapsulate the state (value, status, operator) of a given parameter for a
 * single window.
 * 
 * @author waugh
 *
 */
public class AParameterState implements Cloneable {
	double value;
	boolean status;
	String operator = "";
	
	/**
	 * Make a new AParameterState with default values.
	 */
	AParameterState() {}
	
	/**
	 * Make a new AParameterState that is a copy of another.
	 * Other classes should use the public copy() method.
	 * @param from the object to copy
	 */
	private AParameterState(AParameterState from) {
		this.value = from.value;
		this.status = from.status;
		this.operator = from.operator;
	}
	
	/**
	 * Make a new AParameterState that is a copy of this one.
	 * @return copy
	 */
	public AParameterState copy() {
		return new AParameterState(this);
	}
}
