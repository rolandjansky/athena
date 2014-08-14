package testutils;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * Wraps any number of OutputStreams and directs input to all of them.
 * @author waugh
 *
 */
public class TeeOutputStream extends OutputStream {
	private List<OutputStream> streams;
	
	/**
	 * Create a TeeOutputStream that writes to the given OutputStreams.
	 * @param outputStreams the OutputStreams to be wrapped
	 */
	public TeeOutputStream(OutputStream... outputStreams) {
		streams = new ArrayList<OutputStream>();
		for (OutputStream stream : outputStreams) {
			streams.add(stream);
		}
	}
	
	@Override
	public void write(int b) throws IOException {
		for (OutputStream stream : streams) {
			stream.write(b);
		}
	}

}
