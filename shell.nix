{pkgs ? import <nixos-unstable> {}}:
pkgs.clangStdenv.mkDerivation {
  name = "";
  buildInputs = with pkgs; [ cmake llvm_17 llvmPackages_17.clangUseLLVM ninja ];
}