package atlantis.parameters;

/**
 * Encapsulates the per-window data belonging to a parameter.
 * 
 * @author waugh
 *
 */
public class AParameterData {
	private AParameterState[] state;
	private AParameterState[] defaultState;

    protected final APar parameterStore = APar.instance();

    /**
     * Create AParameterData for given number of windows.
     * @param numWindows number of windows in the canvas
     */
	AParameterData(int numWindows) {
		state = new AParameterState[numWindows];
		defaultState = new AParameterState[numWindows];
		initializeParameterState(state);
		initializeParameterState(defaultState);
	}
	
	/**
	 * Initialize arrays of AParameterState objects.
	 * @param state
	 */
	private static void initializeParameterState(AParameterState[] state) {
		for (int i=0; i<state.length; ++i) {
			state[i] = new AParameterState();
		}
	}
	
	/**
	 * Return the state of this parameter for a given window.
	 * @param windowIndex index of window 
	 * @return the state
	 */
	AParameterState getState(int windowIndex) {
		return state[windowIndex];
	}

	/**
	 * Get value of parameter for current window.
	 * @return value
	 */
	public double getValue() {
        int currentWindowIndex = parameterStore.getCurrentWindowIndex();
		return getValue(currentWindowIndex);
	}

	/**
	 * Get value of parameter for given window.
	 * @param windowIndex index of window
	 * @return value
	 */
	public double getValue(int windowIndex) {
		return state[windowIndex].value;
	}

	/**
	 * Set value of parameter for current window.
	 * @param value
	 */
	public void setValue(double value) {
        int currentWindowIndex = parameterStore.getCurrentWindowIndex();
        setValue(currentWindowIndex,value);
	}

	/**
	 * Set value of parameter for given window.
	 * @param windowIndex index of window
	 * @param value
	 */
	public void setValue(int windowIndex, double value) {
		this.state[windowIndex].value = value;
	}

	/**
	 * Get status (whether parameter is enabled) for current window.
	 * @return true if enabled
	 */
	public boolean getStatus() {
        int currentWindowIndex = parameterStore.getCurrentWindowIndex();
		return getStatus(currentWindowIndex);
	}

	/**
	 * Get status (whether parameter is enabled) for given window.
	 * @param windowIndex index of window
	 * @return true if enabled
	 */
	public boolean getStatus(int windowIndex) {
		return state[windowIndex].status;
	}

	/**
	 * Set status (whether parameter is enabled) for current window.
	 * @param status true to enable
	 */
	public void setStatus(boolean status) {
        int currentWindowIndex = parameterStore.getCurrentWindowIndex();
		setStatus(currentWindowIndex,status);
	}

	/**
	 * Set status (whether parameter is enabled) for given window).
	 * @param windowIndex index of window
	 * @param status true to enable
	 */
	public void setStatus(int windowIndex, boolean status) {
		this.state[windowIndex].status = status;
	}

	/**
	 * Get operator for current window.
	 * @return operator
	 */
	public String getOperator() {
        int currentWindowIndex = parameterStore.getCurrentWindowIndex();
		return getOperator(currentWindowIndex);
	}

	/**
	 * Get operator for given window.
	 * @param windowIndex index of window
	 * @return operator
	 */
	public String getOperator(int windowIndex) {
		return state[windowIndex].operator;
	}

	/**
	 * Set operator for current window.
	 * @param operator
	 */
	public void setOperator(String operator) {
        int currentWindowIndex = parameterStore.getCurrentWindowIndex();
		setOperator(currentWindowIndex,operator);
	}

	/**
	 * Set operator for given window.
	 * @param windowIndex index of window
	 * @param operator
	 */
	public void setOperator(int windowIndex, String operator) {
		this.state[windowIndex].operator = operator;
	}

	/**
	 * Copy values from one window to another.
	 * @param from index of source window
	 * @param to index of destination window
	 */
	public void copy(int from, int to) {
		if (to!=from) state[to] = state[from].copy();
	}

	/**
	 * Save current values for each window to be restored later.
	 */
	public void saveDefaults() {
        int numWindows = state.length;
        for (int iWindow = 0; iWindow<numWindows; ++iWindow) {
        	defaultState[iWindow] = state[iWindow].copy();
        }
	}

	/**
	 * Restore values for each window from saved values.
	 */
	public void restoreDefaults() {
        int numWindows = state.length;
        for (int iWindow = 0; iWindow<numWindows; ++iWindow) {
        	state[iWindow] = defaultState[iWindow].copy();
        }
	}
	
    /**
     * This function is called for GLOBAL parameters to apply the value in all
     * windows.
     */
    public void globalize()
    {
        int currentWindowIndex = parameterStore.getCurrentWindowIndex();
        globalize(currentWindowIndex);
    }

    /**
     * This function is called for GLOBAL parameters to apply the value in all
     * windows.
     */
    public void globalize(int window)
    {
        int numWindows = state.length;
        for (int i = 0; i < numWindows; i++)
            copy(window, i);
    }
}