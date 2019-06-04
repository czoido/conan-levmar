import os

from conans import ConanFile, CMake, tools


class LevmarConan(ConanFile):
    name = "levmar"
    version = "2.6"
    license = "GPL"
    author = "czoido"
    url = "https://github.com/czoido/conan-levmar"
    homepage = "http://users.ics.forth.gr/~lourakis/levmar"
    description = "C/C++ implementation of the Levenberg-Marquardt non-linear least squares algorithm"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    topics = ("Levenberg-Marquardt", "optimization", "algorithm", "algebra")
    generators = "cmake"
    requires = ("libf2c/20181026@czoido/stable")
    _source_subfolder = "levmar_sources"

    def source(self):
        tools.get("{}/{}-{}.tgz".format(self.homepage, self.name, self.version))
        os.rename("{}-{}".format(self.name, self.version), self._source_subfolder)
        # This small hack might be useful to guarantee proper /MT /MD linkage
        # in MSVC if the packaged project doesn't have variables to set it
        # properly
        tools.replace_in_file("%s/CMakeLists.txt" % self._source_subfolder, "PROJECT(LEVMAR)",
                              '''PROJECT(LEVMAR)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()''')
        tools.replace_in_file("%s/CMakeLists.txt" % self._source_subfolder, "TARGET_LINK_LIBRARIES(lmdemo ${LIBS})",
                              "TARGET_LINK_LIBRARIES(lmdemo ${LIBS} gfortran)")

    def build(self):
        cmake = CMake(self)
        cmake.definitions["HAVE_LAPACK"] = False
        cmake.definitions["NEED_F2C"] = True
        cmake.definitions["LM_DBL_PREC"] = True
        cmake.definitions["BUILD_DEMO"] = False
        cmake.configure(source_folder=self._source_subfolder)
        cmake.build()

    def package(self):
        self.copy("LICENSE", dst="licenses", src=self._source_subfolder)
        self.copy("*.h", dst="include", src=self._source_subfolder)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["levmar"]
