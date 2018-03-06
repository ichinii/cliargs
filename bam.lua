
-- targets
PseudoTarget("include")
PseudoTarget("shared", "include")
PseudoTarget("static", "include")
PseudoTarget("test", "static")
PseudoTarget("all", "shared", "static", "include", "test")
DefaultTarget("shared")

-- variables
conf = ScriptArgs["conf"] or "debug"
build_dir = ScriptArgs["build_dir"] or "build"
build_dir = PathJoin(build_dir, conf)
build_shared_dir = PathJoin(build_dir, "shared")
build_static_dir = PathJoin(build_dir, "static")
build_test_dir = PathJoin(build_dir, "test")
build_include_dir = PathJoin(build_dir, "cliargs")
src_dir = "src"
test_dir = "test"
bin_name = ScriptArgs["name"] or "cliargs"

-- configure settings on creation
AddTool(function(settings)
	if conf == "debug" then
		settings.debug = 1
		settings.optimize = 0
	  settings.cc.flags:Add("-g")
	elseif conf == "release" then
		settings.debug = 0
		settings.optimize = 1
	end

	settings.cc.flags:Add("-Wall")
	settings.cc.flags:Add("-Wextra")
	settings.cc.flags_cxx:Add("--std=c++17")
	settings.cc.includes:Add(src_dir)
end)

function MakeSharedSettings(settings)
	settings.cc.flags:Add("-fPIC")
	settings.cc.Output = function(settings, input)
		input = input:gsub("^"..src_dir.."/", "")
		return PathJoin(PathJoin(build_shared_dir, "obj"), PathBase(input))
	end
	settings.dll.Output = function(settings, input) -- shared
		return PathJoin(build_dir, PathBase(input))
	end
	return settings
end

function MakeStaticSettings(settings)
	settings.cc.Output = function(settings, input)
		input = input:gsub("^"..src_dir.."/", "")
		return PathJoin(PathJoin(build_static_dir, "obj"), PathBase(input))
	end
	settings.lib.Output = function(settings, input) -- static
		return PathJoin(build_dir, PathBase(input))
	end
	return settings
end

function MakeTestSettings(settings)
	settings.link.flags:Add("-L./"..build_dir)
	settings.link.flags:Add("-lcliargs")
	settings.cc.Output = function(settings, input)
		input = input:gsub("^"..src_dir.."/", "")
		return PathJoin(PathJoin(build_test_dir, "obj"), PathBase(input))
	end
	settings.link.Output = function(settings, input) --test
		return PathJoin(build_dir, PathBase(input))
	end
	return settings
end

-- settings
settings = NewSettings()
shared_settings = MakeSharedSettings(TableDeepCopy(settings))
static_settings = MakeStaticSettings(TableDeepCopy(settings))
test_settings = MakeTestSettings(TableDeepCopy(settings))

-- jobs for target: include
headers = CollectRecursive(PathJoin(src_dir, "*.hpp"))
for _, file in pairs(headers) do
	local dst_file = file:gsub("^"..src_dir.."/", "")
	dst_file = PathJoin(build_include_dir, dst_file)
	AddJob(dst_file, "cp "..file.." -> "..dst_file, "cp "..file.." "..dst_file)
	AddDependency("include", dst_file)
end

-- compile jobs for targets: shared, static
srcs = CollectRecursive(PathJoin(src_dir, "*.cpp"))
shared_objs = Compile(shared_settings, srcs)
static_objs = Compile(static_settings, srcs)

shared_bin = SharedLibrary(shared_settings, bin_name, shared_objs)
static_bin = StaticLibrary(static_settings, bin_name, static_objs)

AddDependency("shared", shared_bin)
AddDependency("static", static_bin)

test_srcs = CollectRecursive(PathJoin(test_dir, "*.cpp"))
test_objs = Compile(test_settings, test_srcs)
test_bin = Link(test_settings, "test-cliargs", test_objs)
AddDependency("test", test_bin)
