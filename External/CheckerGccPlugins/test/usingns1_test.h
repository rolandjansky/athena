namespace std {}
using namespace std;
using std::qq;
int xx [[gnu::thread_safe]];

namespace foo {}
namespace boost { namespace aux { using namespace foo; } }

