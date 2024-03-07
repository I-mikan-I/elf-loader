{
  description = "ElfLoader development shell";
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  nixConfig.bash-prompt = "[nix(elf-loader):\\w]$ ";

  outputs = { self, nixpkgs }:
    let pkgs = nixpkgs.legacyPackages.x86_64-linux.pkgs;
    in {
      devShells.x86_64-linux.default = import ./shell.nix { inherit pkgs; };
    };
}