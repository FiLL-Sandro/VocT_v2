#include <common.h>
#include <msq_ops.h>

#include "cli/cli.h"
#include "cli/clifilesession.h"

#include "Menu.h"

using namespace cli;
using namespace std;

static const module_id module_id = MENU;
static Log log("Dictionary", Log::LogLevel::LL_DEBUG);
static MenuHandlers menu(log);

int main()
{
	if (common::put_pid("/tmp/Menu.pid") == -1)
	{
		LOG_ERROR("cannot create pid-file\n");
		exit(1);
	}

	if (msq_init(module_id))
	{
		LOG_ERROR("cannot initialize IPC\n");
		exit(1);
	}

	auto rootMenu = make_unique< Menu >( "main" );
	rootMenu -> Insert("open"
	                   , [](std::ostream& out, const std::string &filepath)
	                       { menu.open_handler(out, filepath); }
	                   , "Open dictionary and prepare it for operations"
	                   );
	rootMenu -> Insert("close"
	                   , [](std::ostream& out)
	                       { menu.close_handler(out); }
	                   , "Close previously openned dictionary"
	                   );
	rootMenu -> Insert("add"
	                   , [](std::ostream& out, const std::string &w, const std::string &t)
	                       { menu.add_handler(out, w, t); }
	                   , "Add new translation"
	                   );
	rootMenu -> Insert("rem"
	                   , [](std::ostream& out, const std::string &w)
	                       { menu.rem_handler(out, w); }
	                   , "Remove entry from dictionary"
	                   );
	rootMenu -> Insert("dump"
	                   , [](std::ostream& out)
	                       { menu.dump_handler(out); }
	                   , "Dump content of dictionary"
	                   );

	Cli cli( std::move(rootMenu) );
	cli.EnterAction( [](auto& out){ out << "Welcome to VocT :)\n"; } );
	cli.ExitAction( [](auto& out){ out << "Goodbye, my friend :)\n"; } );

	CliFileSession input(cli);
	input.Start();

	return 0;
}
