# shell.nix
let
  nixpkgs = fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-25.05";
  pkgs = import nixpkgs { config = {}; overlays = []; };
in


pkgs.mkShell {
  packages = with pkgs; 
  [
    llvmPackages_21.libllvm
    llvmPackages_21.clangUseLLVM
    llvmPackages_21.clang-tools
    glibc
    gnumake
    bear
  ];

  buildInputs = with pkgs; [
  ];
}
