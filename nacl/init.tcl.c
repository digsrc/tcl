 "# init.tcl --\n"
 "#\n"
 "# Default system startup file for Tcl-based applications.  Defines\n"
 "# \"unknown\" procedure and auto-load facilities.\n"
 "#\n"
 "# Copyright (c) 1991-1993 The Regents of the University of California.\n"
 "# Copyright (c) 1994-1996 Sun Microsystems, Inc.\n"
 "# Copyright (c) 1998-1999 Scriptics Corporation.\n"
 "# Copyright (c) 2004 by Kevin B. Kenny.  All rights reserved.\n"
 "#\n"
 "# See the file \"license.terms\" for information on usage and redistribution\n"
 "# of this file, and for a DISCLAIMER OF ALL WARRANTIES.\n"
 "#\n"
 "\n"
 "if {[info commands package] == \"\"} {\n"
 "    error \"version mismatch: library\\nscripts expect Tcl version 7.5b1 or later but the loaded version is\\nonly [info patchlevel]\"\n"
 "}\n"
 "package require -exact Tcl 8.6b1.2\n"
 "\n"
 "# Compute the auto path to use in this interpreter.\n"
 "# The values on the path come from several locations:\n"
 "#\n"
 "# The environment variable TCLLIBPATH\n"
 "#\n"
 "# tcl_library, which is the directory containing this init.tcl script.\n"
 "# [tclInit] (Tcl_Init()) searches around for the directory containing this\n"
 "# init.tcl and defines tcl_library to that location before sourcing it.\n"
 "#\n"
 "# The parent directory of tcl_library. Adding the parent\n"
 "# means that packages in peer directories will be found automatically.\n"
 "#\n"
 "# Also add the directory ../lib relative to the directory where the\n"
 "# executable is located.  This is meant to find binary packages for the\n"
 "# same architecture as the current executable.\n"
 "#\n"
 "# tcl_pkgPath, which is set by the platform-specific initialization routines\n"
 "#	On UNIX it is compiled in\n"
 "#       On Windows, it is not used\n"
 "\n"
 "if {![info exists auto_path]} {\n"
 "    if {[info exists env(TCLLIBPATH)]} {\n"
 "	set auto_path $env(TCLLIBPATH)\n"
 "    } else {\n"
 "	set auto_path \"\"\n"
 "    }\n"
 "}\n"
 "namespace eval tcl {\n"
 "    variable Dir\n"
 "    foreach Dir [list $::tcl_library [file dirname $::tcl_library]] {\n"
 "	if {$Dir ni $::auto_path} {\n"
 "	    lappend ::auto_path $Dir\n"
 "	}\n"
 "    }\n"
 "    set Dir [file join [file dirname [file dirname \\\n"
 "	    [info nameofexecutable]]] lib]\n"
 "    if {$Dir ni $::auto_path} {\n"
 "	lappend ::auto_path $Dir\n"
 "    }\n"
 "    catch {\n"
 "	foreach Dir $::tcl_pkgPath {\n"
 "	    if {$Dir ni $::auto_path} {\n"
 "		lappend ::auto_path $Dir\n"
 "	    }\n"
 "	}\n"
 "    }\n"
 "\n"
 "    if {![interp issafe]} {\n"
 "        variable Path [encoding dirs]\n"
 "        set Dir [file join $::tcl_library encoding]\n"
 "        if {$Dir ni $Path} {\n"
 "	    lappend Path $Dir\n"
 "	    encoding dirs $Path\n"
 "        }\n"
 "    }\n"
 "\n"
 "    # TIP #255 min and max functions\n"
 "    namespace eval mathfunc {\n"
 "	proc min {args} {\n"
 "	    if {![llength $args]} {\n"
 "		return -code error \\\n"
 "		    \"too few arguments to math function \\\"min\\\"\"\n"
 "	    }\n"
 "	    set val Inf\n"
 "	    foreach arg $args {\n"
 "		# This will handle forcing the numeric value without\n"
 "		# ruining the internal type of a numeric object\n"
 "		if {[catch {expr {double($arg)}} err]} {\n"
 "		    return -code error $err\n"
 "		}\n"
 "		if {$arg < $val} {set val $arg}\n"
 "	    }\n"
 "	    return $val\n"
 "	}\n"
 "	proc max {args} {\n"
 "	    if {![llength $args]} {\n"
 "		return -code error \\\n"
 "		    \"too few arguments to math function \\\"max\\\"\"\n"
 "	    }\n"
 "	    set val -Inf\n"
 "	    foreach arg $args {\n"
 "		# This will handle forcing the numeric value without\n"
 "		# ruining the internal type of a numeric object\n"
 "		if {[catch {expr {double($arg)}} err]} {\n"
 "		    return -code error $err\n"
 "		}\n"
 "		if {$arg > $val} {set val $arg}\n"
 "	    }\n"
 "	    return $val\n"
 "	}\n"
 "	namespace export min max\n"
 "    }\n"
 "}\n"
 "\n"
 "# Windows specific end of initialization\n"
 "\n"
 "if {(![interp issafe]) && ($tcl_platform(platform) eq \"windows\")} {\n"
 "    namespace eval tcl {\n"
 "	proc EnvTraceProc {lo n1 n2 op} {\n"
 "	    set x $::env($n2)\n"
 "	    set ::env($lo) $x\n"
 "	    set ::env([string toupper $lo]) $x\n"
 "	}\n"
 "	proc InitWinEnv {} {\n"
 "	    global env tcl_platform\n"
 "	    foreach p [array names env] {\n"
 "		set u [string toupper $p]\n"
 "		if {$u ne $p} {\n"
 "		    switch -- $u {\n"
 "			COMSPEC -\n"
 "			PATH {\n"
 "			    if {![info exists env($u)]} {\n"
 "				set env($u) $env($p)\n"
 "			    }\n"
 "			    trace add variable env($p) write \\\n"
 "				    [namespace code [list EnvTraceProc $p]]\n"
 "			    trace add variable env($u) write \\\n"
 "				    [namespace code [list EnvTraceProc $p]]\n"
 "			}\n"
 "		    }\n"
 "		}\n"
 "	    }\n"
 "	    if {![info exists env(COMSPEC)]} {\n"
 "		if {$tcl_platform(os) eq \"Windows NT\"} {\n"
 "		    set env(COMSPEC) cmd.exe\n"
 "		} else {\n"
 "		    set env(COMSPEC) command.com\n"
 "		}\n"
 "	    }\n"
 "	}\n"
 "	InitWinEnv\n"
 "    }\n"
 "}\n"
 "\n"
 "# Setup the unknown package handler\n"
 "\n"
 "\n"
 "if {[interp issafe]} {\n"
 "    package unknown {::tcl::tm::UnknownHandler ::tclPkgUnknown}\n"
 "} else {\n"
 "    # Set up search for Tcl Modules (TIP #189).\n"
 "    # and setup platform specific unknown package handlers\n"
 "    if {$::tcl_platform(os) eq \"Darwin\"\n"
 "	    && $::tcl_platform(platform) eq \"unix\"} {\n"
 "	package unknown {::tcl::tm::UnknownHandler \\\n"
 "		{::tcl::MacOSXPkgUnknown ::tclPkgUnknown}}\n"
 "    } else {\n"
 "	package unknown {::tcl::tm::UnknownHandler ::tclPkgUnknown}\n"
 "    }\n"
 "\n"
 "    # Set up the 'clock' ensemble\n"
 "\n"
 "    namespace eval ::tcl::clock [list variable TclLibDir $::tcl_library]\n"
 "\n"
 "    proc clock args {\n"
 "	namespace eval ::tcl::clock [list namespace ensemble create -command \\\n"
 "		[uplevel 1 [list namespace origin [lindex [info level 0] 0]]] \\\n"
 "		-subcommands {\n"
 "		    add clicks format microseconds milliseconds scan seconds\n"
 "		}]\n"
 "\n"
 "	# Auto-loading stubs for 'clock.tcl'\n"
 "\n"
 "	foreach cmd {add format scan} {\n"
 "	    proc ::tcl::clock::$cmd args {\n"
 "		variable TclLibDir\n"
 "		source -encoding utf-8 [file join $TclLibDir clock.tcl]\n"
 "		return [uplevel 1 [info level 0]]\n"
 "	    }\n"
 "	}\n"
 "\n"
 "	return [uplevel 1 [info level 0]]\n"
 "    }\n"
 "}\n"
 "\n"
 "# Conditionalize for presence of exec.\n"
 "\n"
 "if {[namespace which -command exec] eq \"\"} {\n"
 "\n"
 "    # Some machines do not have exec. Also, on all\n"
 "    # platforms, safe interpreters do not have exec.\n"
 "\n"
 "    set auto_noexec 1\n"
 "}\n"
 "\n"
 "# Define a log command (which can be overwitten to log errors\n"
 "# differently, specially when stderr is not available)\n"
 "\n"
 "if {[namespace which -command tclLog] eq \"\"} {\n"
 "    proc tclLog {string} {\n"
 "	catch {puts stderr $string}\n"
 "    }\n"
 "}\n"
 "\n"
 "# unknown --\n"
 "# This procedure is called when a Tcl command is invoked that doesn't\n"
 "# exist in the interpreter.  It takes the following steps to make the\n"
 "# command available:\n"
 "#\n"
 "#	1. See if the autoload facility can locate the command in a\n"
 "#	   Tcl script file.  If so, load it and execute it.\n"
 "#	2. If the command was invoked interactively at top-level:\n"
 "#	    (a) see if the command exists as an executable UNIX program.\n"
 "#		If so, \"exec\" the command.\n"
 "#	    (b) see if the command requests csh-like history substitution\n"
 "#		in one of the common forms !!, !<number>, or ^old^new.  If\n"
 "#		so, emulate csh's history substitution.\n"
 "#	    (c) see if the command is a unique abbreviation for another\n"
 "#		command.  If so, invoke the command.\n"
 "#\n"
 "# Arguments:\n"
 "# args -	A list whose elements are the words of the original\n"
 "#		command, including the command name.\n"
 "\n"
 "proc unknown args {\n"
 "    variable ::tcl::UnknownPending\n"
 "    global auto_noexec auto_noload env tcl_interactive\n"
 "\n"
 "\n"
 "    if {[info exists ::errorInfo]} {\n"
 "	set savedErrorInfo $::errorInfo\n"
 "    }\n"
 "    if {[info exists ::errorCode]} {\n"
 "	set savedErrorCode $::errorCode\n"
 "    }\n"
 "\n"
 "    set name [lindex $args 0]\n"
 "    if {![info exists auto_noload]} {\n"
 "	#\n"
 "	# Make sure we're not trying to load the same proc twice.\n"
 "	#\n"
 "	if {[info exists UnknownPending($name)]} {\n"
 "	    return -code error \"self-referential recursion\\\n"
 "		    in \\\"unknown\\\" for command \\\"$name\\\"\"\n"
 "	}\n"
 "	set UnknownPending($name) pending\n"
 "	set ret [catch {\n"
 "		auto_load $name [uplevel 1 {::namespace current}]\n"
 "	} msg opts]\n"
 "	unset UnknownPending($name)\n"
 "	if {$ret != 0} {\n"
 "	    dict append opts -errorinfo \"\\n    (autoloading \\\"$name\\\")\"\n"
 "	    return -options $opts $msg\n"
 "	}\n"
 "	if {![array size UnknownPending]} {\n"
 "	    unset UnknownPending\n"
 "	}\n"
 "	if {$msg} {\n"
 "	    if {[info exists savedErrorCode]} {\n"
 "		set ::errorCode $savedErrorCode\n"
 "	    } else {\n"
 "		unset -nocomplain ::errorCode\n"
 "	    }\n"
 "	    if {[info exists savedErrorInfo]} {\n"
 "		set ::errorInfo $savedErrorInfo\n"
 "	    } else {\n"
 "		unset -nocomplain ::errorInfo\n"
 "	    }\n"
 "	    set code [catch {uplevel 1 $args} msg opts]\n"
 "	    if {$code ==  1} {\n"
 "		#\n"
 "		# Compute stack trace contribution from the [uplevel].\n"
 "		# Note the dependence on how Tcl_AddErrorInfo, etc.\n"
 "		# construct the stack trace.\n"
 "		#\n"
 "		set errorInfo [dict get $opts -errorinfo]\n"
 "		set errorCode [dict get $opts -errorcode]\n"
 "		set cinfo $args\n"
 "		if {[string bytelength $cinfo] > 150} {\n"
 "		    set cinfo [string range $cinfo 0 150]\n"
 "		    while {[string bytelength $cinfo] > 150} {\n"
 "			set cinfo [string range $cinfo 0 end-1]\n"
 "		    }\n"
 "		    append cinfo ...\n"
 "		}\n"
 "		append cinfo \"\\\"\\n    (\\\"uplevel\\\" body line 1)\"\n"
 "		append cinfo \"\\n    invoked from within\"\n"
 "		append cinfo \"\\n\\\"uplevel 1 \\$args\\\"\"\n"
 "		#\n"
 "		# Try each possible form of the stack trace\n"
 "		# and trim the extra contribution from the matching case\n"
 "		#\n"
 "		set expect \"$msg\\n    while executing\\n\\\"$cinfo\"\n"
 "		if {$errorInfo eq $expect} {\n"
 "		    #\n"
 "		    # The stack has only the eval from the expanded command\n"
 "		    # Do not generate any stack trace here.\n"
 "		    #\n"
 "		    dict unset opts -errorinfo\n"
 "		    dict incr opts -level\n"
 "		    return -options $opts $msg\n"
 "		}\n"
 "		#\n"
 "		# Stack trace is nested, trim off just the contribution\n"
 "		# from the extra \"eval\" of $args due to the \"catch\" above.\n"
 "		#\n"
 "		set expect \"\\n    invoked from within\\n\\\"$cinfo\"\n"
 "		set exlen [string length $expect]\n"
 "		set eilen [string length $errorInfo]\n"
 "		set i [expr {$eilen - $exlen - 1}]\n"
 "		set einfo [string range $errorInfo 0 $i]\n"
 "		#\n"
 "		# For now verify that $errorInfo consists of what we are about\n"
 "		# to return plus what we expected to trim off.\n"
 "		#\n"
 "		if {$errorInfo ne \"$einfo$expect\"} {\n"
 "		    error \"Tcl bug: unexpected stack trace in \\\"unknown\\\"\" {} \\\n"
 "			[list CORE UNKNOWN BADTRACE $einfo $expect $errorInfo]\n"
 "		}\n"
 "		return -code error -errorcode $errorCode \\\n"
 "			-errorinfo $einfo $msg\n"
 "	    } else {\n"
 "		dict incr opts -level\n"
 "		return -options $opts $msg\n"
 "	    }\n"
 "	}\n"
 "    }\n"
 "\n"
 "    if {([info level] == 1) && ([info script] eq \"\") \\\n"
 "	    && [info exists tcl_interactive] && $tcl_interactive} {\n"
 "	if {![info exists auto_noexec]} {\n"
 "	    set new [auto_execok $name]\n"
 "	    if {$new ne \"\"} {\n"
 "		set redir \"\"\n"
 "		if {[namespace which -command console] eq \"\"} {\n"
 "		    set redir \">&@stdout <@stdin\"\n"
 "		}\n"
 "		uplevel 1 [list ::catch \\\n"
 "			[concat exec $redir $new [lrange $args 1 end]] \\\n"
 "			::tcl::UnknownResult ::tcl::UnknownOptions]\n"
 "		dict incr ::tcl::UnknownOptions -level\n"
 "		return -options $::tcl::UnknownOptions $::tcl::UnknownResult\n"
 "	    }\n"
 "	}\n"
 "	if {$name eq \"!!\"} {\n"
 "	    set newcmd [history event]\n"
 "	} elseif {[regexp {^!(.+)$} $name -> event]} {\n"
 "	    set newcmd [history event $event]\n"
 "	} elseif {[regexp {^\\^([^^]*)\\^([^^]*)\\^?$} $name -> old new]} {\n"
 "	    set newcmd [history event -1]\n"
 "	    catch {regsub -all -- $old $newcmd $new newcmd}\n"
 "	}\n"
 "	if {[info exists newcmd]} {\n"
 "	    tclLog $newcmd\n"
 "	    history change $newcmd 0\n"
 "	    uplevel 1 [list ::catch $newcmd \\\n"
 "		    ::tcl::UnknownResult ::tcl::UnknownOptions]\n"
 "	    dict incr ::tcl::UnknownOptions -level\n"
 "	    return -options $::tcl::UnknownOptions $::tcl::UnknownResult\n"
 "	}\n"
 "\n"
 "	set ret [catch {set candidates [info commands $name*]} msg]\n"
 "	if {$name eq \"::\"} {\n"
 "	    set name \"\"\n"
 "	}\n"
 "	if {$ret != 0} {\n"
 "	    dict append opts -errorinfo \\\n"
 "		    \"\\n    (expanding command prefix \\\"$name\\\" in unknown)\"\n"
 "	    return -options $opts $msg\n"
 "	}\n"
 "	# Filter out bogus matches when $name contained\n"
 "	# a glob-special char [Bug 946952]\n"
 "	if {$name eq \"\"} {\n"
 "	    # Handle empty $name separately due to strangeness\n"
 "	    # in [string first] (See RFE 1243354)\n"
 "	    set cmds $candidates\n"
 "	} else {\n"
 "	    set cmds [list]\n"
 "	    foreach x $candidates {\n"
 "		if {[string first $name $x] == 0} {\n"
 "		    lappend cmds $x\n"
 "		}\n"
 "	    }\n"
 "	}\n"
 "	if {[llength $cmds] == 1} {\n"
 "	    uplevel 1 [list ::catch [lreplace $args 0 0 [lindex $cmds 0]] \\\n"
 "		    ::tcl::UnknownResult ::tcl::UnknownOptions]\n"
 "	    dict incr ::tcl::UnknownOptions -level\n"
 "	    return -options $::tcl::UnknownOptions $::tcl::UnknownResult\n"
 "	}\n"
 "	if {[llength $cmds]} {\n"
 "	    return -code error \"ambiguous command name \\\"$name\\\": [lsort $cmds]\"\n"
 "	}\n"
 "    }\n"
 "    return -code error \"invalid command name \\\"$name\\\"\"\n"
 "}\n"
 "\n"
 "# auto_load --\n"
 "# Checks a collection of library directories to see if a procedure\n"
 "# is defined in one of them.  If so, it sources the appropriate\n"
 "# library file to create the procedure.  Returns 1 if it successfully\n"
 "# loaded the procedure, 0 otherwise.\n"
 "#\n"
 "# Arguments:\n"
 "# cmd -			Name of the command to find and load.\n"
 "# namespace (optional)  The namespace where the command is being used - must be\n"
 "#                       a canonical namespace as returned [namespace current]\n"
 "#                       for instance. If not given, namespace current is used.\n"
 "\n"
 "proc auto_load {cmd {namespace {}}} {\n"
 "    global auto_index auto_path\n"
 "\n"
 "    if {$namespace eq \"\"} {\n"
 "	set namespace [uplevel 1 [list ::namespace current]]\n"
 "    }\n"
 "    set nameList [auto_qualify $cmd $namespace]\n"
 "    # workaround non canonical auto_index entries that might be around\n"
 "    # from older auto_mkindex versions\n"
 "    lappend nameList $cmd\n"
 "    foreach name $nameList {\n"
 "	if {[info exists auto_index($name)]} {\n"
 "	    namespace eval :: $auto_index($name)\n"
 "	    # There's a couple of ways to look for a command of a given\n"
 "	    # name.  One is to use\n"
 "	    #    info commands $name\n"
 "	    # Unfortunately, if the name has glob-magic chars in it like *\n"
 "	    # or [], it may not match.  For our purposes here, a better\n"
 "	    # route is to use\n"
 "	    #    namespace which -command $name\n"
 "	    if {[namespace which -command $name] ne \"\"} {\n"
 "		return 1\n"
 "	    }\n"
 "	}\n"
 "    }\n"
 "    if {![info exists auto_path]} {\n"
 "	return 0\n"
 "    }\n"
 "\n"
 "    if {![auto_load_index]} {\n"
 "	return 0\n"
 "    }\n"
 "    foreach name $nameList {\n"
 "	if {[info exists auto_index($name)]} {\n"
 "	    namespace eval :: $auto_index($name)\n"
 "	    if {[namespace which -command $name] ne \"\"} {\n"
 "		return 1\n"
 "	    }\n"
 "	}\n"
 "    }\n"
 "    return 0\n"
 "}\n"
 "\n"
 "# auto_load_index --\n"
 "# Loads the contents of tclIndex files on the auto_path directory\n"
 "# list.  This is usually invoked within auto_load to load the index\n"
 "# of available commands.  Returns 1 if the index is loaded, and 0 if\n"
 "# the index is already loaded and up to date.\n"
 "#\n"
 "# Arguments:\n"
 "# None.\n"
 "\n"
 "proc auto_load_index {} {\n"
 "    variable ::tcl::auto_oldpath\n"
 "    global auto_index auto_path\n"
 "\n"
 "    if {[info exists auto_oldpath] && ($auto_oldpath eq $auto_path)} {\n"
 "	return 0\n"
 "    }\n"
 "    set auto_oldpath $auto_path\n"
 "\n"
 "    # Check if we are a safe interpreter. In that case, we support only\n"
 "    # newer format tclIndex files.\n"
 "\n"
 "    set issafe [interp issafe]\n"
 "    for {set i [expr {[llength $auto_path] - 1}]} {$i >= 0} {incr i -1} {\n"
 "	set dir [lindex $auto_path $i]\n"
 "	set f \"\"\n"
 "	if {$issafe} {\n"
 "	    catch {source [file join $dir tclIndex]}\n"
 "	} elseif {[catch {set f [open [file join $dir tclIndex]]}]} {\n"
 "	    continue\n"
 "	} else {\n"
 "	    set error [catch {\n"
 "		set id [gets $f]\n"
 "		if {$id eq \"# Tcl autoload index file, version 2.0\"} {\n"
 "		    eval [read $f]\n"
 "		} elseif {$id eq \"# Tcl autoload index file: each line identifies a Tcl\"} {\n"
 "		    while {[gets $f line] >= 0} {\n"
 "			if {([string index $line 0] eq \"#\") \\\n"
 "				|| ([llength $line] != 2)} {\n"
 "			    continue\n"
 "			}\n"
 "			set name [lindex $line 0]\n"
 "			set auto_index($name) \\\n"
 "				\"source [file join $dir [lindex $line 1]]\"\n"
 "		    }\n"
 "		} else {\n"
 "		    error \"[file join $dir tclIndex] isn't a proper Tcl index file\"\n"
 "		}\n"
 "	    } msg opts]\n"
 "	    if {$f ne \"\"} {\n"
 "		close $f\n"
 "	    }\n"
 "	    if {$error} {\n"
 "		return -options $opts $msg\n"
 "	    }\n"
 "	}\n"
 "    }\n"
 "    return 1\n"
 "}\n"
 "\n"
 "# auto_qualify --\n"
 "#\n"
 "# Compute a fully qualified names list for use in the auto_index array.\n"
 "# For historical reasons, commands in the global namespace do not have leading\n"
 "# :: in the index key. The list has two elements when the command name is\n"
 "# relative (no leading ::) and the namespace is not the global one. Otherwise\n"
 "# only one name is returned (and searched in the auto_index).\n"
 "#\n"
 "# Arguments -\n"
 "# cmd		The command name. Can be any name accepted for command\n"
 "#               invocations (Like \"foo::::bar\").\n"
 "# namespace	The namespace where the command is being used - must be\n"
 "#               a canonical namespace as returned by [namespace current]\n"
 "#               for instance.\n"
 "\n"
 "proc auto_qualify {cmd namespace} {\n"
 "\n"
 "    # count separators and clean them up\n"
 "    # (making sure that foo:::::bar will be treated as foo::bar)\n"
 "    set n [regsub -all {::+} $cmd :: cmd]\n"
 "\n"
 "    # Ignore namespace if the name starts with ::\n"
 "    # Handle special case of only leading ::\n"
 "\n"
 "    # Before each return case we give an example of which category it is\n"
 "    # with the following form :\n"
 "    # (inputCmd, inputNameSpace) -> output\n"
 "\n"
 "    if {[string match ::* $cmd]} {\n"
 "	if {$n > 1} {\n"
 "	    # (::foo::bar , *) -> ::foo::bar\n"
 "	    return [list $cmd]\n"
 "	} else {\n"
 "	    # (::global , *) -> global\n"
 "	    return [list [string range $cmd 2 end]]\n"
 "	}\n"
 "    }\n"
 "\n"
 "    # Potentially returning 2 elements to try  :\n"
 "    # (if the current namespace is not the global one)\n"
 "\n"
 "    if {$n == 0} {\n"
 "	if {$namespace eq \"::\"} {\n"
 "	    # (nocolons , ::) -> nocolons\n"
 "	    return [list $cmd]\n"
 "	} else {\n"
 "	    # (nocolons , ::sub) -> ::sub::nocolons nocolons\n"
 "	    return [list ${namespace}::$cmd $cmd]\n"
 "	}\n"
 "    } elseif {$namespace eq \"::\"} {\n"
 "	#  (foo::bar , ::) -> ::foo::bar\n"
 "	return [list ::$cmd]\n"
 "    } else {\n"
 "	# (foo::bar , ::sub) -> ::sub::foo::bar ::foo::bar\n"
 "	return [list ${namespace}::$cmd ::$cmd]\n"
 "    }\n"
 "}\n"
 "\n"
 "# auto_import --\n"
 "#\n"
 "# Invoked during \"namespace import\" to make see if the imported commands\n"
 "# reside in an autoloaded library.  If so, the commands are loaded so\n"
 "# that they will be available for the import links.  If not, then this\n"
 "# procedure does nothing.\n"
 "#\n"
 "# Arguments -\n"
 "# pattern	The pattern of commands being imported (like \"foo::*\")\n"
 "#               a canonical namespace as returned by [namespace current]\n"
 "\n"
 "proc auto_import {pattern} {\n"
 "    global auto_index\n"
 "\n"
 "    # If no namespace is specified, this will be an error case\n"
 "\n"
 "    if {![string match *::* $pattern]} {\n"
 "	return\n"
 "    }\n"
 "\n"
 "    set ns [uplevel 1 [list ::namespace current]]\n"
 "    set patternList [auto_qualify $pattern $ns]\n"
 "\n"
 "    auto_load_index\n"
 "\n"
 "    foreach pattern $patternList {\n"
 "        foreach name [array names auto_index $pattern] {\n"
 "            if {([namespace which -command $name] eq \"\")\n"
 "		    && ([namespace qualifiers $pattern] eq [namespace qualifiers $name])} {\n"
 "                namespace eval :: $auto_index($name)\n"
 "            }\n"
 "        }\n"
 "    }\n"
 "}\n"
 "\n"
 "# auto_execok --\n"
 "#\n"
 "# Returns string that indicates name of program to execute if\n"
 "# name corresponds to a shell builtin or an executable in the\n"
 "# Windows search path, or \"\" otherwise.  Builds an associative\n"
 "# array auto_execs that caches information about previous checks,\n"
 "# for speed.\n"
 "#\n"
 "# Arguments:\n"
 "# name -			Name of a command.\n"
 "\n"
 "if {$tcl_platform(platform) eq \"windows\"} {\n"
 "# Windows version.\n"
 "#\n"
 "# Note that info executable doesn't work under Windows, so we have to\n"
 "# look for files with .exe, .com, or .bat extensions.  Also, the path\n"
 "# may be in the Path or PATH environment variables, and path\n"
 "# components are separated with semicolons, not colons as under Unix.\n"
 "#\n"
 "proc auto_execok name {\n"
 "    global auto_execs env tcl_platform\n"
 "\n"
 "    if {[info exists auto_execs($name)]} {\n"
 "	return $auto_execs($name)\n"
 "    }\n"
 "    set auto_execs($name) \"\"\n"
 "\n"
 "    set shellBuiltins [list cls copy date del erase dir echo mkdir \\\n"
 "	    md rename ren rmdir rd time type ver vol]\n"
 "    if {$tcl_platform(os) eq \"Windows NT\"} {\n"
 "	# NT includes the 'start' built-in\n"
 "	lappend shellBuiltins \"start\"\n"
 "    }\n"
 "    if {[info exists env(PATHEXT)]} {\n"
 "	# Add an initial ; to have the {} extension check first.\n"
 "	set execExtensions [split \";$env(PATHEXT)\" \";\"]\n"
 "    } else {\n"
 "	set execExtensions [list {} .com .exe .bat .cmd]\n"
 "    }\n"
 "\n"
 "    if {$name in $shellBuiltins} {\n"
 "	# When this is command.com for some reason on Win2K, Tcl won't\n"
 "	# exec it unless the case is right, which this corrects.  COMSPEC\n"
 "	# may not point to a real file, so do the check.\n"
 "	set cmd $env(COMSPEC)\n"
 "	if {[file exists $cmd]} {\n"
 "	    set cmd [file attributes $cmd -shortname]\n"
 "	}\n"
 "	return [set auto_execs($name) [list $cmd /c $name]]\n"
 "    }\n"
 "\n"
 "    if {[llength [file split $name]] != 1} {\n"
 "	foreach ext $execExtensions {\n"
 "	    set file ${name}${ext}\n"
 "	    if {[file exists $file] && ![file isdirectory $file]} {\n"
 "		return [set auto_execs($name) [list $file]]\n"
 "	    }\n"
 "	}\n"
 "	return \"\"\n"
 "    }\n"
 "\n"
 "    set path \"[file dirname [info nameof]];.;\"\n"
 "    if {[info exists env(WINDIR)]} {\n"
 "	set windir $env(WINDIR)\n"
 "    }\n"
 "    if {[info exists windir]} {\n"
 "	if {$tcl_platform(os) eq \"Windows NT\"} {\n"
 "	    append path \"$windir/system32;\"\n"
 "	}\n"
 "	append path \"$windir/system;$windir;\"\n"
 "    }\n"
 "\n"
 "    foreach var {PATH Path path} {\n"
 "	if {[info exists env($var)]} {\n"
 "	    append path \";$env($var)\"\n"
 "	}\n"
 "    }\n"
 "\n"
 "    foreach dir [split $path {;}] {\n"
 "	# Skip already checked directories\n"
 "	if {[info exists checked($dir)] || ($dir eq \"\")} {\n"
 "	    continue\n"
 "	}\n"
 "	set checked($dir) {}\n"
 "	foreach ext $execExtensions {\n"
 "	    set file [file join $dir ${name}${ext}]\n"
 "	    if {[file exists $file] && ![file isdirectory $file]} {\n"
 "		return [set auto_execs($name) [list $file]]\n"
 "	    }\n"
 "	}\n"
 "    }\n"
 "    return \"\"\n"
 "}\n"
 "\n"
 "} else {\n"
 "# Unix version.\n"
 "#\n"
 "proc auto_execok name {\n"
 "    global auto_execs env\n"
 "\n"
 "    if {[info exists auto_execs($name)]} {\n"
 "	return $auto_execs($name)\n"
 "    }\n"
 "    set auto_execs($name) \"\"\n"
 "    if {[llength [file split $name]] != 1} {\n"
 "	if {[file executable $name] && ![file isdirectory $name]} {\n"
 "	    set auto_execs($name) [list $name]\n"
 "	}\n"
 "	return $auto_execs($name)\n"
 "    }\n"
 "    foreach dir [split $env(PATH) :] {\n"
 "	if {$dir eq \"\"} {\n"
 "	    set dir .\n"
 "	}\n"
 "	set file [file join $dir $name]\n"
 "	if {[file executable $file] && ![file isdirectory $file]} {\n"
 "	    set auto_execs($name) [list $file]\n"
 "	    return $auto_execs($name)\n"
 "	}\n"
 "    }\n"
 "    return \"\"\n"
 "}\n"
 "\n"
 "}\n"
 "\n"
 "# ::tcl::CopyDirectory --\n"
 "#\n"
 "# This procedure is called by Tcl's core when attempts to call the\n"
 "# filesystem's copydirectory function fail.  The semantics of the call\n"
 "# are that 'dest' does not yet exist, i.e. dest should become the exact\n"
 "# image of src.  If dest does exist, we throw an error.\n"
 "#\n"
 "# Note that making changes to this procedure can change the results\n"
 "# of running Tcl's tests.\n"
 "#\n"
 "# Arguments:\n"
 "# action -              \"renaming\" or \"copying\"\n"
 "# src -			source directory\n"
 "# dest -		destination directory\n"
 "proc tcl::CopyDirectory {action src dest} {\n"
 "    set nsrc [file normalize $src]\n"
 "    set ndest [file normalize $dest]\n"
 "\n"
 "    if {$action eq \"renaming\"} {\n"
 "	# Can't rename volumes.  We could give a more precise\n"
 "	# error message here, but that would break the test suite.\n"
 "	if {$nsrc in [file volumes]} {\n"
 "	    return -code error \"error $action \\\"$src\\\" to\\\n"
 "	      \\\"$dest\\\": trying to rename a volume or move a directory\\\n"
 "	      into itself\"\n"
 "	}\n"
 "    }\n"
 "    if {[file exists $dest]} {\n"
 "	if {$nsrc eq $ndest} {\n"
 "	    return -code error \"error $action \\\"$src\\\" to\\\n"
 "	      \\\"$dest\\\": trying to rename a volume or move a directory\\\n"
 "	      into itself\"\n"
 "	}\n"
 "	if {$action eq \"copying\"} {\n"
 "	    # We used to throw an error here, but, looking more closely\n"
 "	    # at the core copy code in tclFCmd.c, if the destination\n"
 "	    # exists, then we should only call this function if -force\n"
 "	    # is true, which means we just want to over-write.  So,\n"
 "	    # the following code is now commented out.\n"
 "	    #\n"
 "	    # return -code error \"error $action \\\"$src\\\" to\\\n"
 "	    # \\\"$dest\\\": file already exists\"\n"
 "	} else {\n"
 "	    # Depending on the platform, and on the current\n"
 "	    # working directory, the directories '.', '..'\n"
 "	    # can be returned in various combinations.  Anyway,\n"
 "	    # if any other file is returned, we must signal an error.\n"
 "	    set existing [glob -nocomplain -directory $dest * .*]\n"
 "	    lappend existing {*}[glob -nocomplain -directory $dest \\\n"
 "		    -type hidden * .*]\n"
 "	    foreach s $existing {\n"
 "		if {([file tail $s] ne \".\") && ([file tail $s] ne \"..\")} {\n"
 "		    return -code error \"error $action \\\"$src\\\" to\\\n"
 "		      \\\"$dest\\\": file already exists\"\n"
 "		}\n"
 "	    }\n"
 "	}\n"
 "    } else {\n"
 "	if {[string first $nsrc $ndest] != -1} {\n"
 "	    set srclen [expr {[llength [file split $nsrc]] -1}]\n"
 "	    set ndest [lindex [file split $ndest] $srclen]\n"
 "	    if {$ndest eq [file tail $nsrc]} {\n"
 "		return -code error \"error $action \\\"$src\\\" to\\\n"
 "		  \\\"$dest\\\": trying to rename a volume or move a directory\\\n"
 "		  into itself\"\n"
 "	    }\n"
 "	}\n"
 "	file mkdir $dest\n"
 "    }\n"
 "    # Have to be careful to capture both visible and hidden files.\n"
 "    # We will also be more generous to the file system and not\n"
 "    # assume the hidden and non-hidden lists are non-overlapping.\n"
 "    #\n"
 "    # On Unix 'hidden' files begin with '.'.  On other platforms\n"
 "    # or filesystems hidden files may have other interpretations.\n"
 "    set filelist [concat [glob -nocomplain -directory $src *] \\\n"
 "      [glob -nocomplain -directory $src -types hidden *]]\n"
 "\n"
 "    foreach s [lsort -unique $filelist] {\n"
 "	if {([file tail $s] ne \".\") && ([file tail $s] ne \"..\")} {\n"
 "	    file copy -force $s [file join $dest [file tail $s]]\n"
 "	}\n"
 "    }\n"
 "    return\n"
 "}\n"
