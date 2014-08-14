package guitest;

import static org.junit.Assert.*;

import java.util.Arrays;

/**
 * Useful assertion methods for tests.
 * @author waugh
 *
 */
public class AAssert {
	public static void assertArrayEqualsIgnoreOrder(String message,
			Object[] expecteds, Object[] actuals) {
		int lengthExp = expecteds.length;
		int lengthAct = actuals.length;
		if (lengthAct!=lengthExp) fail(message+
				": array lengths differed, "+
				String.format("expected.length=%d,actual.length=%d",lengthExp,lengthAct));
		Object[] copyExpecteds = new Object[lengthExp];
		System.arraycopy(expecteds,0,copyExpecteds,0,lengthExp);
		Object[] copyActuals = new Object[lengthAct];
		System.arraycopy(actuals,0,copyActuals,0,lengthAct);
		Arrays.sort(copyExpecteds);
		Arrays.sort(copyActuals);
		assertArrayEquals(message,copyExpecteds,copyActuals);
	}
}
