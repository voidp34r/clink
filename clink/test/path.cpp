// Copyright (c) 2015 Martin Ridgers
// License: http://opensource.org/licenses/MIT

#include "pch.h"

#include <core/path.h>
#include <core/str.h>

TEST_CASE("Paths") {
    str<> s, t;

    SECTION("path::clean()") {
        s.copy("X://0/\\/1/2//\\3/\\\\//4//");
        path::clean(s);
        REQUIRE(s.equals("X:\\0\\1\\2\\3\\4\\"));

        path::clean(s, '/');
        REQUIRE(s.equals("X:/0/1/2/3/4/"));

        s.copy("abcdef");
        path::clean(s);
        REQUIRE(s.equals("abcdef"));
    }

    SECTION("path::get_base_name()") {
        SECTION("Basic") {
            SECTION("0") { path::get_base_name("one/two/three/filename.ext", s); }
            SECTION("1") { path::get_base_name("one/two/three/filename.ext", s); }
            SECTION("2") { path::get_base_name("one/two/three\\filename.ext", s); }
            SECTION("3") { path::get_base_name("filename.ext", s); }
            SECTION("4") { path::get_base_name("filename", s); }
            SECTION("5") { path::get_base_name("c:filename.ext", s); }
            SECTION("6") { path::get_base_name("c:filename", s); }

            REQUIRE(s.equals("filename"));
        }

        SECTION("Other") {
            path::get_base_name("one/two/three/filename...ext", s);
            REQUIRE(s.equals("filename.."));
            s.clear();

            path::get_base_name("one/two/three/filename..ext.ext", s);
            REQUIRE(s.equals("filename..ext"));
            s.clear();

            path::get_base_name("one/two/three/filename...", s);
            REQUIRE(s.equals("filename.."));
            s.clear();

            path::get_base_name("one/two/three/filename.ext.", s);
            REQUIRE(s.equals("filename.ext"));
            s.clear();
        }

    }

    SECTION("path::get_directory()") {
        SECTION("Copy") {
            SECTION("0") { t << "one/two/three/filename.ext"; }
            SECTION("1") { t << "one/two/three\\filename.ext"; }
            SECTION("2") { t << "one/two/three/"; }
            SECTION("3") { t << "one/two/three\\"; }
            SECTION("4") { t << "one/two/three///"; }
            SECTION("5") { t << "one/two/three\\\\\\"; }

            path::get_directory(t.c_str(), s);
            path::clean(s, '/');
            REQUIRE(s.equals("one/two/three"));
        }

        SECTION("In-place") {
            SECTION("0") { s << "one/two/three/filename.ext"; }
            SECTION("1") { s << "one/two\\three/filename.ext"; }

            path::get_directory(s);
            path::clean(s, '/');
            REQUIRE(s.equals("one/two/three"));
        }

        SECTION("No slash") {
            SECTION("0") { s << "one"; }
            SECTION("1") { s << ""; }

            path::get_directory(s);
            path::clean(s, '/');
            REQUIRE(s.equals(""));
        }

        SECTION("Root basic") {
            SECTION("0") { t << "/"; }
            SECTION("1") { t << "\\"; }
            SECTION("2") { t << "/one"; }
            SECTION("3") { t << "\\one"; }

            path::get_directory(t.c_str(), s);
            path::clean(s, '/');
            REQUIRE(s.equals("/"));
        }

        SECTION("Root in-place") {
            SECTION("0") { s << "/"; }
            SECTION("1") { s << "\\"; }
            SECTION("2") { s << "/one"; }
            SECTION("3") { s << "\\one"; }

            path::get_directory(s);
            path::clean(s, '/');
            REQUIRE(s.equals("/"));
        }

        SECTION("Drive letter") {
            s.clear();
            path::get_directory("e:\\one", s);
            REQUIRE(s.equals("e:\\"));

            s.clear();
            path::get_directory("e:/one", s);
            REQUIRE(s.equals("e:/"));

            s.clear();
            path::get_directory("e:one", s);
            REQUIRE(s.equals("e:"));
        }
    }

    SECTION("path::get_drive()") {
        SECTION("Has drive") {
            SECTION("0") { s << "e:"; }
            SECTION("1") { s << "e:/"; }
            SECTION("2") { s << "e:/one/filename.ext"; }
            SECTION("3") { s << "e:one/filename.ext"; }
            SECTION("4") { s << "E:\\one/filename.ext"; }
            SECTION("5") { s << "E:one/filename.ext"; }

            REQUIRE(path::get_drive(s.c_str(), t));
            REQUIRE((t.equals("e:") || t.equals("E:")));
            REQUIRE(path::get_drive(t));
        }

        SECTION("No drive") {
            SECTION("0") { s << ""; }
            SECTION("1") { s << ":"; }
            SECTION("2") { s << ":/"; }
            SECTION("3") { s << "[:/"; }
            SECTION("4") { s << "{:"; }
            SECTION("5") { s << "@:"; }
            SECTION("6") { s << "`:\\"; }
            SECTION("7") { s << "/one/filename.ext"; }
            SECTION("8") { s << "one/filename.ext"; }
            SECTION("9") { s << "filename.ext"; }

            REQUIRE(!path::get_drive(s.c_str(), t));
            REQUIRE(!path::get_drive(s));
        }
    }

    SECTION("path::get_extension()") {
        SECTION("Has extension") {
            SECTION("0") { path::get_extension("one/two/three/filename.ext", s); }
            SECTION("1") { path::get_extension("one/two/three/filename.ext", s); }
            SECTION("2") { path::get_extension("one/two/three\\filename.ext", s); }
            SECTION("3") { path::get_extension("filename.ext", s); }
            SECTION("3") { path::get_extension("filename..ext", s); }
            SECTION("4") { path::get_extension(".ext", s); }

            REQUIRE(s.equals(".ext"));
        }

        SECTION("Misc") {
            SECTION("0") {
                path::get_extension("..", s);
                REQUIRE(s.equals("."));
            }
            SECTION("1") { REQUIRE(!path::get_extension("", s)); }
            SECTION("2") { REQUIRE(!path::get_extension("abc", s)); }
        }
    }

    SECTION("path::get_name()") {
        SECTION("Basic") {
            SECTION("0") { path::get_name("one/two/three/filename.ext", s); }
            SECTION("1") { path::get_name("one/two/three\\filename.ext", s); }
            SECTION("2") { path::get_name("filename.ext", s); }

            REQUIRE(s.equals("filename.ext"));
        }
    }

    SECTION("path::is_root()") {
        SECTION("True") {
            REQUIRE(path::is_root("e:"));
            REQUIRE(path::is_root("e:/"));
            REQUIRE(path::is_root("e:\\"));
            REQUIRE(path::is_root("/"));
            REQUIRE(path::is_root("\\"));
            REQUIRE(path::is_root(""));
        }

        SECTION("False") {
            REQUIRE(!path::is_root("e:one"));
            REQUIRE(!path::is_root("e:/one"));
            REQUIRE(!path::is_root("e:\\one"));
            REQUIRE(!path::is_root("/one"));
            REQUIRE(!path::is_root("\\one"));
            REQUIRE(!path::is_root("one"));
        }
    }

    SECTION("path::join()") {
        SECTION("Basic") {
            SECTION("0") { path::join("one/two", "three/four", s); }
            SECTION("1") { path::join("one/two/", "three/four", s); }
            SECTION("2") { path::join("one/two\\", "three/four", s); }
            SECTION("3") { path::join("one/two", "/three/four", s); }
            SECTION("4") { path::join("one/two", "\\three/four", s); }

            path::clean(s);
            REQUIRE(s.equals("one\\two\\three\\four"));
        }

        SECTION("One side empty") {
            SECTION("Left") {
                path::join("", "one\\two", s);
                REQUIRE(s.equals("one\\two"));
            }

            SECTION("Right") {
                path::join("one\\two", "", s);
                REQUIRE(s.equals("one\\two\\"));
            }
        }

        SECTION("Drive letter") {
            // Absolute
            s.copy("x:");
            path::append(s, "/one");
            REQUIRE(s.equals("x:/one"));

            s.copy("x:/");
            path::append(s, "one");
            REQUIRE(s.equals("x:/one"));

            s.copy("x:");
            path::append(s, "\\one");
            REQUIRE(s.equals("x:\\one"));

            s.copy("x:\\");
            path::append(s, "one");
            REQUIRE(s.equals("x:\\one"));

            // Relative
            s.copy("x:");
            path::append(s, "one");
            REQUIRE(s.equals("x:one"));
        }
    }
}
