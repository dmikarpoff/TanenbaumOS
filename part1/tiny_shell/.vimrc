set tabstop=2
set softtabstop=2
set shiftwidth=2
set expandtab
set colorcolumn=110
highlight ColorColumn ctermbg=darkgray

augroup project
  autocmd!
  autocmd BufRead,BufNewFile *.h,*.c set filetype=c
augroup END
