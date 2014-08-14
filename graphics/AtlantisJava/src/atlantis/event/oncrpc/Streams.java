package atlantis.event.oncrpc;
import org.acplt.oncrpc.*;
import java.io.IOException;

/**
 * Data structure as returned from ONCRPC_GETSTREAMS_PROC is a list of stream names
 * available at the server.
 * @author Sebastian Boeser
 */
public class Streams implements XdrAble {

    /** The array of stream names */
    public String [] StreamNames;

    /**
     * Constructor
     */
    public Streams() {
    //Array is empty by default, nothing to initialize
    }

    /**
     * Constructure that can be passed to an RPC client call
     * @param xdr the stream to decode from
     * @throws OncRpcException if decoding fails
     * @throws IOException
     */
    public Streams(XdrDecodingStream xdr) throws OncRpcException, IOException {
      xdrDecode(xdr);
    }

    /**
     * Encode the information in XDR representation
     * @param xdr the XDR stream to encode into
     * @throws OncRpcException if encoding fails
     * @throws IOException
     */
    public void xdrEncode(XdrEncodingStream xdr) throws OncRpcException, IOException {
      //Encode the stream name
      xdr.xdrEncodeStringVector(StreamNames);
    }

    /**
     * Decode the information from XDR representation
     * @param xdr the XDR stream to decode from
     * @throws OncRpcException if decoding fails
     * @throws IOException
     */
    public void xdrDecode(XdrDecodingStream xdr) throws OncRpcException, IOException {
      //Encode stream names
      StreamNames = xdr.xdrDecodeStringVector();
    }
}
